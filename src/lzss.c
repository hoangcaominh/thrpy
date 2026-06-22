#include "lzss.h"
#include "bitstream.h"
#include <stdio.h>
#include <string.h>

#define LZSS_INDEX_SIZE         13
#define LZSS_LENGTH_SIZE 		4
#define LZSS_DICT_SIZE 			(1 << LZSS_INDEX_SIZE)
#define LZSS_DICT_MASK 		    (LZSS_DICT_SIZE - 1)
#define LZSS_BREAK_EVEN         3
#define LZSS_LOOK_AHEAD_SIZE 	((1 << LZSS_LENGTH_SIZE) + LZSS_BREAK_EVEN - 1)
#define LZSS_INIT_WRITE_INDEX 	1
#define LZSS_DICT_MODPOS(pos)   ((pos) & (LZSS_DICT_MASK))

struct tree_node {
    int parent;
    int left;
    int right;
};

void tree_init(struct tree_node* tree, int root) {
    tree[LZSS_DICT_SIZE].right = root;
    tree[root].parent = LZSS_DICT_SIZE;
    tree[root].left = tree[root].right = 0;
}

void tree_contract_node(struct tree_node* tree, int old_node, int new_node) {
    tree[new_node].parent = tree[old_node].parent;
    if (tree[tree[old_node].parent].right == old_node)
        tree[tree[old_node].parent].right = new_node;
    else
        tree[tree[old_node].parent].left = new_node;
    tree[old_node].parent = 0;
}

void tree_replace_node(struct tree_node* tree, int old_node, int new_node) {
    int parent = tree[old_node].parent;
    if (tree[parent].left == old_node)
        tree[parent].left = new_node;
    else
        tree[parent].right = new_node;
    tree[new_node] = tree[old_node];
    tree[tree[new_node].left].parent = tree[tree[new_node].right].parent = new_node;
    tree[old_node].parent = 0;
}

int tree_find_next_node(struct tree_node* tree, int node) {
    int next = tree[node].left;
    while (tree[next].right != 0)
        next = tree[next].right;
    return next;
}

void tree_delete_string(struct tree_node* tree, int node) {
    if (tree[node].parent == 0)
        return;
    if (tree[node].right == 0)
        tree_contract_node(tree, node, tree[node].left);
    else if (tree[node].left == 0)
        tree_contract_node(tree, node, tree[node].right);
    else {
        int replacement = tree_find_next_node(tree, node);
        tree_delete_string(tree, replacement);
        tree_replace_node(tree, node, replacement);
    }
}

int tree_add_string(struct tree_node* tree, uint8_t* dict, int new_node, int* match_pos) {
    if (new_node == 0)
        return 0;
    int delta;
    int i;
    int* child;
    int test_node = tree[LZSS_DICT_SIZE].right;
    int match_len = 0;
    while (1) {
        for (i = 0; i < LZSS_LOOK_AHEAD_SIZE; i++) {
            delta = dict[LZSS_DICT_MODPOS(new_node + i)] - dict[LZSS_DICT_MODPOS(test_node + i)];
            if (delta != 0)
                break;
        }
        if (i >= match_len) {
            match_len = i;
            *match_pos = test_node;
            if (match_len >= LZSS_LOOK_AHEAD_SIZE) {
                tree_replace_node(tree, test_node, new_node);
                return match_len;
            }
        }
        if (delta >= 0)
            child = &tree[test_node].right;
        else
            child = &tree[test_node].left;
        if (*child == 0) {
            *child = new_node;
            tree[new_node].parent = test_node;
            tree[new_node].left = tree[new_node].right = 0;
            return match_len;
        }
        test_node = *child;
    }
}

size_t rpy_unpack(uint8_t* data, size_t size, uint8_t* out, size_t outsize) {
    if (!data || !out)
        return 0;

    struct bitstream* bs = bitstream_init_reader(data, size);
    if (!bs)
        return 0;

    uint8_t* tmp = (uint8_t*)calloc(outsize, sizeof(*tmp));
    if (!tmp) {
        bitstream_destroy(bs);
        return 0;
    }

	uint8_t dict[LZSS_DICT_SIZE];
	int dict_head = LZSS_INIT_WRITE_INDEX;
    size_t bytes_written = 0;
	uint8_t c;

	memset(dict, 0, sizeof(dict));

	while (1) {
		if (bitstream_read(bs, 1)) {
			c = bitstream_read(bs, 8);
            tmp[bytes_written++] = c;
            if (bitstream_eos(bs) || bytes_written >= outsize)
                goto loop_end;
            dict[dict_head] = c;
            dict_head = LZSS_DICT_MODPOS(dict_head + 1);
		} else {
			int match_pos = bitstream_read(bs, LZSS_INDEX_SIZE);
			if (!match_pos)
				break;
			int match_len = bitstream_read(bs, LZSS_LENGTH_SIZE);
			match_len += LZSS_BREAK_EVEN;

			for (int i = 0; i < match_len; i++) {
				c = dict[LZSS_DICT_MODPOS(match_pos + i)];
                tmp[bytes_written++] = c;
                if (bitstream_eos(bs) || bytes_written >= outsize)
                    goto loop_end;
                dict[dict_head] = c;
                dict_head = LZSS_DICT_MODPOS(dict_head + 1);
			}
		}
	}
loop_end:
    if (!bitstream_eos(bs))
        fprintf(stderr, "LZSS data read bytes mismatch or out buffer is too small\n");
    bitstream_destroy(bs);

    mempcpy(out, tmp, outsize);
    free(tmp);
    return bytes_written;
}

size_t rpy_pack(uint8_t* data, size_t size, uint8_t* out, size_t outsize) {
    struct bitstream* bs = bitstream_init_writer();
    if (!bs)
        return 0;
    bitstream_reserve(bs, outsize);

    struct tree_node tree[LZSS_DICT_SIZE + 1];
    uint8_t dict[LZSS_DICT_SIZE];
    int dict_head = LZSS_INIT_WRITE_INDEX;
    int i;
    int replace_count;
    uint8_t* it = data;

    memset(tree, 0, sizeof(tree));
    memset(dict, 0, sizeof(dict));

    for (i = 0; i < LZSS_LOOK_AHEAD_SIZE && it < data + size; i++) {
        dict[dict_head + i] = *it;
        it++;
    }

    int look_ahead_bytes = i;
    tree_init(tree, dict_head);
    // int match_len = LZSS_BREAK_EVEN - 1;
    int match_len = 0;
    int match_pos = 0;

    while (look_ahead_bytes > 0) {
        if (match_len > look_ahead_bytes)
            match_len = look_ahead_bytes;
        if (match_len < LZSS_BREAK_EVEN) {
            bitstream_write(bs, 1);
            bitstream_write_n(bs, 8, dict[dict_head]);
            replace_count = 1;
        } else {
            bitstream_write(bs, 0);
            bitstream_write_n(bs, LZSS_INDEX_SIZE, match_pos);
            bitstream_write_n(bs, LZSS_LENGTH_SIZE, match_len - LZSS_BREAK_EVEN);
            replace_count = match_len;
        }

        for (i = 0; i < replace_count; i++) {
            const int pos = LZSS_DICT_MODPOS(dict_head + LZSS_LOOK_AHEAD_SIZE);
            tree_delete_string(tree, pos);

            if (it < data + size) {
                dict[pos] = *it;
                it++;
            } else {
                look_ahead_bytes--;
            }

            dict_head = LZSS_DICT_MODPOS(dict_head + 1);
            if (look_ahead_bytes > 0)
                match_len = tree_add_string(tree, dict, dict_head, &match_pos);
        }
    }

    // bitstream_write(bs, 0);
    // bitstream_write(bs, LZSS_INDEX_SIZE, 0);

    // Add a NULL byte at the end of buffer
    bitstream_write_n(bs, 8, 0);

    size_t bytes_written = bitstream_get_data(bs, out, outsize);
    bitstream_destroy(bs);

    return bytes_written;
}
