#include "lzss.h"
#include <cstdio>
#include <cstring>

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

class BitStream {
public:
    BitStream()
        : buf(RpyBuf()), bitcount(0), pos(0) {}
    BitStream(RpyBuf::const_iterator begin, RpyBuf::const_iterator end)
        : buf(begin, end), bitcount((end - begin) * 8), pos(0) {}

    RpyBuf get_buf() const {
        return this->buf;
    }

    unsigned int read(size_t n) {
        if (n > 32)
            return 0;

        unsigned int ret = 0;
        uint8_t byte, mask;

        for (size_t i = 0; i < n && !this->eos(); i++, this->pos++) {
            byte = this->buf[this->pos / 8];
            mask = 1 << (7 - this->pos % 8);
            if (byte & mask)
                ret |= 1 << (n - i - 1);
        }

        return ret;
    }

    void write(bool bit) {
        if (this->pos >= this->buf.size() * 8)
            this->buf.push_back(0x0);

        RpyBuf::iterator it = this->buf.begin() + this->pos / 8;
        uint8_t mask = bit << (7 - this->pos % 8);
        *it |= mask;
        this->pos++;
        this->bitcount++;
    }

    void write(size_t n, uint32_t val) {
        for (size_t i = 0; i < n; i++)
            write((val >> (n - i - 1)) & 1);
    }

    bool eos() const {
        return this->pos >= this->bitcount;
    }
private:
    RpyBuf buf;
    size_t bitcount;
    size_t pos;
};

RpyBuf rpy_decompress(RpyBuf::const_iterator begin, RpyBuf::const_iterator end) {
	uint8_t dict[LZSS_DICT_SIZE];
	int dict_head = LZSS_INIT_WRITE_INDEX;
	uint8_t c;
    BitStream bs(begin, end);
    RpyBuf ret;

	memset(dict, 0, sizeof(dict));

	while (1) {
        if (bs.eos())
            break;
		if (bs.read(1)) {
			c = bs.read(8);
            ret.push_back(c);
            if (bs.eos())
                break;
            dict[dict_head] = c;
            dict_head = LZSS_DICT_MODPOS(dict_head + 1);
		} else {
			int match_pos = bs.read(LZSS_INDEX_SIZE);
			if (!match_pos)
				break;
			int match_len = bs.read(LZSS_LENGTH_SIZE);
			match_len += LZSS_BREAK_EVEN;

			for (int i = 0; i < match_len; i++) {
				c = dict[LZSS_DICT_MODPOS(match_pos + i)];
                ret.push_back(c);
                if (bs.eos())
                    break;
                dict[dict_head] = c;
                dict_head = LZSS_DICT_MODPOS(dict_head + 1);
			}
		}
	}

    if (!bs.eos())
        fprintf(stderr, "LZSS data read bytes mismatch\n");

    return ret;
}

RpyBuf rpy_compress(RpyBuf::const_iterator begin, RpyBuf::const_iterator end) {
    struct tree_node tree[LZSS_DICT_SIZE + 1];
    uint8_t dict[LZSS_DICT_SIZE];
    int dict_head = LZSS_INIT_WRITE_INDEX;
    int i;
    int replace_count;

    BitStream bs;
    memset(tree, 0, sizeof(tree));
    memset(dict, 0, sizeof(dict));

    for (i = 0; i < LZSS_LOOK_AHEAD_SIZE && begin < end; i++) {
        dict[dict_head + i] = *begin;
        begin++;
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
            bs.write(1);
            bs.write(8, dict[dict_head]);
            replace_count = 1;
        } else {
            bs.write(0);
            bs.write(LZSS_INDEX_SIZE, match_pos);
            bs.write(LZSS_LENGTH_SIZE, match_len - LZSS_BREAK_EVEN);
            replace_count = match_len;
        }

        for (i = 0; i < replace_count; i++) {
            const int pos = LZSS_DICT_MODPOS(dict_head + LZSS_LOOK_AHEAD_SIZE);
            tree_delete_string(tree, pos);

            if (begin < end) {
                dict[pos] = *begin;
                begin++;
            } else {
                look_ahead_bytes--;
            }

            dict_head = LZSS_DICT_MODPOS(dict_head + 1);
            if (look_ahead_bytes != 0)
                match_len = tree_add_string(tree, dict, dict_head, &match_pos);
        }
    }

    // bs.write(0);
    // bs.write(LZSS_INDEX_SIZE, 0);

    // Add a NULL byte at the end of buffer
    bs.write(8, 0);

    return bs.get_buf();
}
