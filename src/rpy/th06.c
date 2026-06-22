#include "rpy/th06.h"
#include "crypt.h"
#include <string.h>

static const size_t KEY_OFFSET = 14;
static const size_t LZSS_OFFSET = 15;

void rpybuf_decompile_th06(const RpyBuf* buf, RpyBuf* out) {
    if (!buf || !buf->data || !out || buf->size < LZSS_OFFSET)
        return;

    size_t ptrsize = buf->size;
    uint8_t* ptr = (uint8_t*)calloc(ptrsize, sizeof(*ptr));
    if (!ptr)
        return;
    memcpy(ptr, buf->data, buf->size);

    rpy_decrypt06(ptr + LZSS_OFFSET, ptrsize - LZSS_OFFSET, ptr[KEY_OFFSET]);

    if (out->data)
        free(out->data);
    out->data = ptr;
    out->size = ptrsize;
    out->capacity = ptrsize;
}

void rpybuf_compile_th06(const RpyBuf* buf, RpyBuf* out) {
    if (!buf || !buf->data || !out || buf->size < LZSS_OFFSET)
        return;

    size_t ptrsize = buf->size;
    uint8_t* ptr = (uint8_t*)calloc(ptrsize, sizeof(*ptr));
    if (!ptr)
        return;
    memcpy(ptr, buf->data, buf->size);

    rpy_encrypt06(ptr + LZSS_OFFSET, ptrsize - LZSS_OFFSET, ptr[KEY_OFFSET]);

    if (out->data)
        free(out->data);
    out->data = ptr;
    out->size = ptrsize;
    out->capacity = ptrsize;
}

void rpy_th06(Rpy* rpy) {
    if (!rpy)
        return;
    rpy->decompile = rpybuf_decompile_th06;
    rpy->compile = rpybuf_compile_th06;
}
