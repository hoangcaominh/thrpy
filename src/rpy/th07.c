#include "rpy/th07.h"
#include "crypt.h"
#include "lzss.h"
#include <string.h>

static const size_t KEY_OFFSET = 13;
static const size_t CRYPT_OFFSET = 16;
static const size_t LZSS_OFFSET = 84;

void rpybuf_unpack_th07(const RpyBuf* buf, RpyBuf* out) {
    if (!buf || !buf->data || !out || buf->size < LZSS_OFFSET)
        return;

    // Approximate memory needed for decompressed data
    size_t ptrsize = buf->size * 10;
    uint8_t* ptr = (uint8_t*)calloc(ptrsize, sizeof(*ptr));
    if (!ptr)
        return;
    memcpy(ptr, buf->data, buf->size);

    rpy_decrypt06(ptr + CRYPT_OFFSET, buf->size - CRYPT_OFFSET, ptr[KEY_OFFSET]);
    size_t decomp_size = rpy_unpack(ptr + LZSS_OFFSET, buf->size - LZSS_OFFSET, ptr + LZSS_OFFSET, ptrsize - LZSS_OFFSET);

    if (out->data)
        free(out->data);
    out->data = ptr;
    out->size = LZSS_OFFSET + decomp_size;
    out->capacity = ptrsize;
}

void rpybuf_pack_th07(const RpyBuf* buf, RpyBuf* out) {
    if (!buf || !buf->data || !out || buf->size < LZSS_OFFSET)
        return;

    // Approximate memory needed for decompressed data
    size_t ptrsize = buf->size / 2;
    uint8_t* ptr = (uint8_t*)calloc(ptrsize, sizeof(*ptr));
    if (!ptr)
        return;
    memcpy(ptr, buf->data, LZSS_OFFSET);

    size_t comp_size = rpy_pack(buf->data + LZSS_OFFSET, buf->size - LZSS_OFFSET, ptr + LZSS_OFFSET, ptrsize - LZSS_OFFSET);
    rpy_encrypt06(ptr + CRYPT_OFFSET, LZSS_OFFSET - CRYPT_OFFSET + comp_size, ptr[KEY_OFFSET]);

    if (out->data)
        free(out->data);
    out->data = ptr;
    out->size = LZSS_OFFSET + comp_size;
    out->capacity = ptrsize;
}

void rpy_th07(Rpy* rpy) {
    if (!rpy)
        return;
    rpy->unpack = rpybuf_unpack_th07;
    rpy->pack = rpybuf_pack_th07;
}
