#include "thmodern.h"
#include "crypt.h"
#include "lzss.h"
#include <stdio.h>
#include <string.h>

static const size_t COMP_LEN_OFFSET = 28;
static const size_t DECOMP_LEN_OFFSET = 32;
static const size_t DATA_OFFSET = 36;

size_t unpack_thmodern(RpyBuf* buf, RpyModernKey* key1, RpyModernKey* key2) {
    if (!buf || buf->size < DATA_OFFSET || buf->capacity < DATA_OFFSET)
        return 0;

    uint8_t* ptr_data = buf->data + DATA_OFFSET;
    size_t comp_size = *(uint32_t*)(buf->data + COMP_LEN_OFFSET);
    size_t userdata_offset = DATA_OFFSET + comp_size;
    size_t userdata_size = buf->size - userdata_offset;

    // Backup userdata section
    uint8_t* userdata = (uint8_t*)calloc(userdata_size, sizeof(*userdata));
    if (!userdata)
        return 0;
    memcpy(userdata, buf->data + userdata_offset, userdata_size);

    rpy_decrypt(ptr_data, comp_size, key1);
    rpy_decrypt(ptr_data, comp_size, key2);
    size_t decomp_size = rpy_unlzss(ptr_data, comp_size, ptr_data, buf->capacity - DATA_OFFSET);
    memcpy(
        ptr_data + decomp_size,
        userdata,
        userdata_size
    );
    free(userdata);

    buf->size = buf->size - comp_size + decomp_size;
    return buf->size;
}

size_t pack_thmodern(RpyBuf* buf, RpyModernKey* key1, RpyModernKey* key2) {
    if (!buf || buf->size < DATA_OFFSET || buf->capacity < DATA_OFFSET)
        return 0;

    uint8_t* ptr_data = buf->data + DATA_OFFSET;
    size_t decomp_size = *(uint32_t*)(buf->data + DECOMP_LEN_OFFSET);
    size_t userdata_offset = DATA_OFFSET + decomp_size;

    size_t comp_size = rpy_lzss_no_padding(ptr_data, decomp_size, ptr_data, buf->capacity - DATA_OFFSET);
    *(uint32_t*)(buf->data + COMP_LEN_OFFSET) = comp_size;
    rpy_encrypt(ptr_data, comp_size, key2);
    rpy_encrypt(ptr_data, comp_size, key1);
    memmove(
        ptr_data + comp_size,
        buf->data + userdata_offset,
        buf->size - userdata_offset
    );

    buf->size = buf->size - decomp_size + comp_size;
    return buf->size;
}
