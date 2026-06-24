#include "rpy/th08.h"
#include "crypt.h"
#include "lzss.h"
#include <string.h>

static const size_t KEY_OFFSET = 21;
static const size_t CRYPT_OFFSET = 24;
static const size_t LZSS_OFFSET = 104;

void rpybuf_unpack_th08(const RpyBuf* buf, RpyBuf* out) {
    if (!buf || !buf->data || !out || buf->size < LZSS_OFFSET)
        return;

    // Approximate memory needed for decompressed data
    size_t ptrsize = buf->size * 10;
    uint8_t* ptr = (uint8_t*)calloc(ptrsize, sizeof(*ptr));
    if (!ptr)
        return;
    memcpy(ptr, buf->data, buf->size);

    uint32_t userdata_offset = *(uint32_t*)(buf->data + 12);
    uint32_t comp_size = userdata_offset - LZSS_OFFSET;
    rpy_decrypt06(ptr + CRYPT_OFFSET, userdata_offset - CRYPT_OFFSET, ptr[KEY_OFFSET]);
    size_t decomp_size = rpy_unpack(ptr + LZSS_OFFSET, comp_size, ptr + LZSS_OFFSET, ptrsize - LZSS_OFFSET);
    memcpy(ptr + LZSS_OFFSET + decomp_size, buf->data + userdata_offset, buf->size - userdata_offset);

    if (out->data)
        free(out->data);
    out->data = ptr;
    out->size = buf->size - comp_size + decomp_size;
    out->capacity = ptrsize;
}

void rpybuf_pack_th08(const RpyBuf* buf, RpyBuf* out) {
    if (!buf || !buf->data || !out || buf->size < LZSS_OFFSET)
        return;

    // Approximate memory needed for decompressed data
    size_t ptrsize = buf->size / 2;
    uint8_t* ptr = (uint8_t*)calloc(ptrsize, sizeof(*ptr));
    if (!ptr)
        return;
    memcpy(ptr, buf->data, LZSS_OFFSET);

    // Could also use USER magic
    uint32_t decomp_size = *(uint32_t*)(buf->data + 28);
    uint32_t userdata_offset = LZSS_OFFSET + decomp_size;
    size_t userdata_size = buf->size - userdata_offset;

    size_t comp_size = rpy_pack(buf->data + LZSS_OFFSET, decomp_size, ptr + LZSS_OFFSET, ptrsize - LZSS_OFFSET);
    rpy_encrypt06(ptr + CRYPT_OFFSET, LZSS_OFFSET - CRYPT_OFFSET + comp_size, ptr[KEY_OFFSET]);
    memcpy(ptr + LZSS_OFFSET + comp_size, buf->data + userdata_offset, userdata_size);

    if (out->data)
        free(out->data);
    out->data = ptr;
    out->size = buf->size - decomp_size + comp_size;
    out->capacity = ptrsize;
}

void rpy_th08(Rpy* rpy) {
    if (!rpy)
        return;
    rpy->unpack = rpybuf_unpack_th08;
    rpy->pack = rpybuf_pack_th08;
}
