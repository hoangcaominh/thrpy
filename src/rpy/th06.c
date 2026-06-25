#include "rpy/th06.h"
#include "crypt.h"

static const size_t KEY_OFFSET = 14;
static const size_t CRYPT_OFFSET = 15;

size_t unpack_th06(RpyBuf* buf) {
    if (!buf || buf->size < CRYPT_OFFSET)
        return 0;

    rpy_decrypt06(
        buf->data + CRYPT_OFFSET,
        buf->size - CRYPT_OFFSET,
        buf->data[KEY_OFFSET]
    );
    return buf->size;
}

size_t pack_th06(RpyBuf* buf) {
    if (!buf || buf->size < CRYPT_OFFSET)
        return 0;

    rpy_encrypt06(
        buf->data + CRYPT_OFFSET,
        buf->size - CRYPT_OFFSET,
        buf->data[KEY_OFFSET]
    );
    return buf->size;
}

void rpy_th06(Rpy* rpy) {
    if (!rpy)
        return;
    rpy->unpack_fn = unpack_th06;
    rpy->pack_fn = pack_th06;
}
