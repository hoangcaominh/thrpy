#include "bitstream.h"
#include <stdio.h>
#include <string.h>

struct bitstream {
    uint8_t* data;
    size_t capacity;
    size_t bitcount;
    size_t pos;
};

struct bitstream* bitstream_init_reader(const uint8_t* data, size_t size) {
    struct bitstream* reader = (struct bitstream*)malloc(sizeof(*reader));
    if (!reader)
        return NULL;

    if ((reader->data = (uint8_t*)calloc(size, sizeof(*reader->data))) == NULL) {
        free(reader);
        return NULL;
    }
    memcpy(reader->data, data, size);
    reader->capacity = size;
    reader->bitcount = size * 8;
    reader->pos = 0;

    return reader;
}

bool bitstream_eos(struct bitstream* bs) {
    if (!bs)
        return true;
    return bs->pos >= bs->bitcount;
}

unsigned int bitstream_read(struct bitstream* bs, size_t n) {
    if (!bs)
        return 0;

    unsigned int ret = 0;
    if (n > sizeof(ret) * 8)
        return 0;

    uint8_t byte, mask;
    for (size_t i = 0; i < n && !bitstream_eos(bs); i++, bs->pos++) {
        byte = bs->data[bs->pos / 8];
        mask = 1 << (7 - bs->pos % 8);
        if (byte & mask)
            ret |= 1 << (n - i - 1);
    }

    return ret;
} 

struct bitstream* bitstream_init_writer() {
    struct bitstream* writer = (struct bitstream*)malloc(sizeof(*writer));
    if (!writer)
        return NULL;

    writer->data = NULL;
    writer->capacity = 0;
    writer->bitcount = 0;
    writer->pos = 0;

    return writer;
}

size_t bitstream_get_size(struct bitstream* bs) {
    if (!bs)
        return 0;
    return (bs->bitcount + 7) / 8;
}

bool bitstream_reserve(struct bitstream* bs, size_t newsize) {
    if (!bs)
        return false;

    void* data = reallocarray(bs->data, newsize, 1);
    if (!data)
        return false;

    if (newsize > bs->capacity) {
        memset(data + bs->capacity, 0, newsize - bs->capacity);
    }
    // bs->bitcount
    bs->capacity = newsize;
    bs->data = data;

    return true;
}

void bitstream_write(struct bitstream* bs, bool bit) {
    if (!bs)
        return;

    if (bs->bitcount >= bs->capacity * 8)
        bitstream_reserve(bs, bs->capacity + 1);

    uint8_t* it = bs->data + bs->pos / 8;
    uint8_t mask = bit << (7 - bs->pos % 8);
    *it |= mask;
    bs->pos++;
    bs->bitcount++;
}

void bitstream_write_n(struct bitstream* bs, size_t n, unsigned int val) {
    if (!bs)
        return;

    for (size_t i = 0; i < n; i++)
        bitstream_write(bs, (val >> (n - i - 1)) & 1);
}

size_t bitstream_get_data(struct bitstream* bs, uint8_t* data, size_t size) {
    if (!bs || !data)
        return 0;

    size_t bytes_count = bitstream_get_size(bs);
    if (bytes_count > size)
        bytes_count = size;
    memcpy(data, bs->data, bytes_count);

    return bytes_count;
}

void bitstream_destroy(struct bitstream* bs) {
    if (!bs)
        return;
    free(bs->data);
    free(bs);
}
