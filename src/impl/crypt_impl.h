#pragma once

#include <stdint.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

void decrypt06(uint8_t* buf, size_t size, uint8_t key);
void encrypt06(uint8_t* buf, size_t size, uint8_t key);
void decrypt(uint8_t* buf, size_t size, size_t block_size, uint8_t base, uint8_t add);
void encrypt(uint8_t* buf, size_t size, size_t block_size, uint8_t base, uint8_t add);

#ifdef __cplusplus
}
#endif
