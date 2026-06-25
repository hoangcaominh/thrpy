#include "rpy/th06.h"
#include "crypt.h"
#include <string.h>

static const size_t KEY_OFFSET = 14;
static const size_t LZSS_OFFSET = 15;

size_t rpybuf_unpack_th06(const RpyBuf* buf, RpyBuf* out) {
    if (!buf || !buf->data || !out || buf->size < LZSS_OFFSET)
        return 0;

    size_t ptrsize = buf->size;
    uint8_t* ptr = (uint8_t*)calloc(ptrsize, sizeof(*ptr));
    if (!ptr)
        return 0;
    memcpy(ptr, buf->data, buf->size);

    rpy_decrypt06(ptr + LZSS_OFFSET, ptrsize - LZSS_OFFSET, ptr[KEY_OFFSET]);

    if (out->data)
        free(out->data);
    out->data = ptr;
    out->size = ptrsize;
    out->capacity = ptrsize;

    return out->size;
}

size_t rpybuf_pack_th06(const RpyBuf* buf, RpyBuf* out) {
    if (!buf || !buf->data || !out || buf->size < LZSS_OFFSET)
        return 0;

    size_t ptrsize = buf->size;
    uint8_t* ptr = (uint8_t*)calloc(ptrsize, sizeof(*ptr));
    if (!ptr)
        return 0;
    memcpy(ptr, buf->data, buf->size);

    rpy_encrypt06(ptr + LZSS_OFFSET, ptrsize - LZSS_OFFSET, ptr[KEY_OFFSET]);

    if (out->data)
        free(out->data);
    out->data = ptr;
    out->size = ptrsize;
    out->capacity = ptrsize;

    return out->size;
}

void rpy_th06(Rpy* rpy) {
    if (!rpy)
        return;
    rpy->unpack_fn = rpybuf_unpack_th06;
    rpy->pack_fn = rpybuf_pack_th06;
}
