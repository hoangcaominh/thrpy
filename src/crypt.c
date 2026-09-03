#include "crypt.h"
#include <string.h>

typedef void (*crypt06_func)(uint8_t*, const uint8_t);

static inline void crypt06_func_decrypt(uint8_t* c, const uint8_t key) {
    *c -= key;
}

static inline void crypt06_func_encrypt(uint8_t* c, const uint8_t key) {
    *c += key;
}

void crypt06(uint8_t* data, size_t size, uint8_t key, crypt06_func func) {
    for (uint8_t* it = data; it != data + size; it++) {
        func(it, key);
        key += 7;
    }
}

void rpy_decrypt06(uint8_t* data, size_t size, uint8_t key) {
    crypt06(data, size, key, crypt06_func_decrypt);
}

void rpy_encrypt06(uint8_t* data, size_t size, uint8_t key) {
    crypt06(data, size, key, crypt06_func_encrypt);
}

typedef void (*crypt_func)(uint8_t*, uint8_t*, const uint8_t);

static inline void crypt_func_decrypt(uint8_t* a, uint8_t* b, const uint8_t base) {
    *a = *b ^ base;
}

static inline void crypt_func_encrypt(uint8_t* a, uint8_t* b, const uint8_t base) {
    *b = *a ^ base;
}

void crypt(uint8_t* data, size_t size, RpyModernKey* key, crypt_func func) {
	if (!data || !key)
		return;

    uint8_t* tmp = (uint8_t*)calloc(size, sizeof(*tmp));
    if (!tmp)
        return;
	memcpy(tmp, data, size);

	size_t block_size = key->block_size;
	uint8_t base = key->base;
	uint8_t add = key->add;

	int i, p = 0, tp1, tp2, hf, left = size;
	if ((left % block_size) < (block_size / 4))
		left -= left % block_size;
	left -= size & 1;

	while (left) {
		if (left < block_size)
			block_size = left;

		tp1 = p + block_size - 1;
		tp2 = p + block_size - 2;
		hf = (block_size + (block_size & 0x1)) / 2;
		for (i = 0; i < hf; ++i, ++p) {
            func(data + tp1, tmp + p, base);
			base += add;
			tp1 -= 2;
		}
		hf = block_size / 2;
		for (i = 0; i < hf; ++i, ++p) {
            func(data + tp2, tmp + p, base);
			base += add;
			tp2 -= 2;
		}
		left -= block_size;
	}

    free(tmp);
}

void rpy_decrypt(uint8_t* data, size_t size, RpyModernKey* key) {
    crypt(data, size, key, crypt_func_decrypt);
}

void rpy_encrypt(uint8_t* data, size_t size, RpyModernKey* key) {
    crypt(data, size, key, crypt_func_encrypt);
}

