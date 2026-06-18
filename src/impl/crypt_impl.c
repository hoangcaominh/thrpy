#include "crypt_impl.h"

typedef void (*crypt06_func)(uint8_t*, const uint8_t);

static inline void crypt06_func_decrypt(uint8_t* c, const uint8_t key) {
    *c -= key;
}

static inline void crypt06_func_encrypt(uint8_t* c, const uint8_t key) {
    *c += key;
}

void crypt06(uint8_t* buf, size_t size, uint8_t key, crypt06_func func) {
    for (uint8_t* it = buf; it != buf + size; it++) {
        func(it, key);
        key += 7;
    }
}

void decrypt06(uint8_t* buf, size_t size, uint8_t key) {
    crypt06(buf, size, key, crypt06_func_decrypt);
}

void encrypt06(uint8_t* buf, size_t size, uint8_t key) {
    crypt06(buf, size, key, crypt06_func_encrypt);
}

typedef void (*crypt_func)(uint8_t*, uint8_t*, const uint8_t);

static inline void crypt_func_decrypt(uint8_t* b, uint8_t* t, const uint8_t base) {
    *b = *t ^ base;
}

static inline void crypt_func_encrypt(uint8_t* b, uint8_t* t, const uint8_t base) {
    *t = *b ^ base;
}

void crypt(uint8_t* buf, size_t size, size_t block_size, uint8_t base, uint8_t add, crypt_func func) {
    uint8_t* tbuf = (uint8_t*)calloc(size, sizeof(*tbuf));
    if (!tbuf)
        return;

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
            func(buf + tp1, tbuf + p, base);
			base += add;
			tp1 -= 2;
		}
		hf = block_size / 2;
		for (i = 0; i < hf; ++i, ++p) {
            func(buf + tp2, tbuf + p, base);
			base += add;
			tp2 -= 2;
		}
		left -= block_size;
	}

    free(tbuf);
}

void decrypt(uint8_t* buf, size_t size, size_t block_size, uint8_t base, uint8_t add) {
    crypt(buf, size, block_size, base, add, crypt_func_decrypt);
}

void encrypt(uint8_t* buf, size_t size, size_t block_size, uint8_t base, uint8_t add) {
    crypt(buf, size, block_size, base, add, crypt_func_encrypt);
}

