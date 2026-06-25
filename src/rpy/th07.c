#include "rpy/th07.h"
#include "crypt.h"
#include "lzss.h"

static const size_t KEY_OFFSET = 13;
static const size_t CRYPT_OFFSET = 16;
static const size_t LZSS_OFFSET = 84;

size_t unpack_th07(RpyBuf* buf) {
    if (!buf || buf->size < LZSS_OFFSET || buf->capacity < LZSS_OFFSET)
        return 0;

    rpy_decrypt06(
        buf->data + CRYPT_OFFSET,
        buf->size - CRYPT_OFFSET,
        buf->data[KEY_OFFSET]
    );
    size_t decomp_size = rpy_unlzss(
        buf->data + LZSS_OFFSET,
        buf->size - LZSS_OFFSET,
        buf->data + LZSS_OFFSET,
        buf->capacity - LZSS_OFFSET
    );

    buf->size = LZSS_OFFSET + decomp_size;
    return buf->size;
}

size_t pack_th07(RpyBuf* buf) {
    if (!buf || buf->size < LZSS_OFFSET || buf->capacity < LZSS_OFFSET)
        return 0;

    size_t comp_size = rpy_lzss(
        buf->data + LZSS_OFFSET,
        buf->size - LZSS_OFFSET,
        buf->data + LZSS_OFFSET,
        buf->capacity - LZSS_OFFSET
    );
    rpy_encrypt06(
        buf->data + CRYPT_OFFSET,
        LZSS_OFFSET - CRYPT_OFFSET + comp_size,
        buf->data[KEY_OFFSET]
    );

    buf->size = LZSS_OFFSET + comp_size;
    return buf->size;
}

void rpy_th07(Rpy* rpy) {
    if (!rpy)
        return;
    rpy->unpack_fn = unpack_th07;
    rpy->pack_fn = pack_th07;
}
