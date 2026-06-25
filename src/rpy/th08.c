#include "rpy/th08.h"
#include "crypt.h"
#include "lzss.h"
#include <string.h>

static const size_t KEY_OFFSET = 21;
static const size_t CRYPT_OFFSET = 24;
static const size_t LZSS_OFFSET = 104;

size_t rpybuf_unpack_th08(RpyBuf* buf) {
    if (!buf || buf->size < LZSS_OFFSET || buf->capacity < LZSS_OFFSET)
        return 0;

    uint32_t userdata_offset = *(uint32_t*)(buf->data + 12);
    if (buf->size < userdata_offset)
        return 0;

    uint32_t comp_size = userdata_offset - LZSS_OFFSET;
    uint8_t* ptr_lzss = buf->data + LZSS_OFFSET;
    rpy_decrypt06(
        buf->data + CRYPT_OFFSET,
        userdata_offset - CRYPT_OFFSET,
        buf->data[KEY_OFFSET]
    );
    size_t decomp_size = rpy_unlzss(
        ptr_lzss,
        comp_size,
        ptr_lzss,
        buf->capacity - LZSS_OFFSET
    );
    memcpy(
        ptr_lzss + decomp_size,
        buf->data + userdata_offset,
        buf->size - userdata_offset
    );

    buf->size = buf->size - comp_size + decomp_size;
    return buf->size;
}

size_t rpybuf_pack_th08(RpyBuf* buf) {
    if (!buf || buf->size < LZSS_OFFSET || buf->capacity < LZSS_OFFSET)
        return 0;

    // Could also use USER magic
    uint32_t decomp_size = *(uint32_t*)(buf->data + 28);
    uint32_t userdata_offset = LZSS_OFFSET + decomp_size;
    uint8_t* ptr_lzss = buf->data + LZSS_OFFSET;
    size_t comp_size = rpy_lzss(
        ptr_lzss,
        decomp_size,
        ptr_lzss,
        buf->capacity - LZSS_OFFSET
    );
    rpy_decrypt06(
        buf->data + CRYPT_OFFSET,
        userdata_offset - CRYPT_OFFSET,
        buf->data[KEY_OFFSET]
    );
    memcpy(
        ptr_lzss + comp_size,
        buf->data + userdata_offset,
        buf->size - userdata_offset
    );

    buf->size = buf->size - decomp_size + comp_size;
    return buf->size;
}

void rpy_th08(Rpy* rpy) {
    if (!rpy)
        return;
    rpy->unpack_fn = rpybuf_unpack_th08;
    rpy->pack_fn = rpybuf_pack_th08;
}
