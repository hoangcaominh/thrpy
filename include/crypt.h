#pragma once

#include <stdint.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

void rpy_decrypt06(uint8_t* data, size_t size, uint8_t key);
void rpy_encrypt06(uint8_t* data, size_t size, uint8_t key);
void rpy_decrypt(uint8_t* data, size_t size, size_t block_size, uint8_t base, uint8_t add);
void rpy_encrypt(uint8_t* data, size_t size, size_t block_size, uint8_t base, uint8_t add);

#ifdef __cplusplus
}
#endif
