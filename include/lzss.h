#pragma once

#include <stdint.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

size_t rpy_decompress(uint8_t* data, size_t size, uint8_t* out, size_t outsize);
size_t rpy_compress(uint8_t* data, size_t size, uint8_t* out, size_t outsize);

#ifdef __cplusplus
}
#endif

