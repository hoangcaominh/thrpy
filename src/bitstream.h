#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

struct bitstream;

#ifdef __cplusplus
extern "C" {
#endif

struct bitstream* bitstream_init_reader(const uint8_t* data, size_t size);
bool bitstream_eos(struct bitstream* bs);
unsigned int bitstream_read(struct bitstream* bs, size_t n);

struct bitstream* bitstream_init_writer();
size_t bitstream_get_size(struct bitstream* bs);
bool bitstream_reserve(struct bitstream* bs, size_t newsize);
void bitstream_write(struct bitstream* bs, bool bit);
void bitstream_write_n(struct bitstream* bs, size_t n, unsigned int val);
size_t bitstream_get_data(struct bitstream* bs, uint8_t* data, size_t size);

void bitstream_destroy(struct bitstream* bs);

#ifdef __cplusplus
}
#endif
