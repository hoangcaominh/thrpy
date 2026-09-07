#include "rpy/th128.h"
#include "thmodern.h"

static RpyModernKey key1 = { 0x800, 0x5E, 0xE7 };
static RpyModernKey key2 = { 0x80, 0x7D, 0x36 };

static size_t unpack(RpyBuf* buf) {
    return unpack_thmodern(buf, &key1, &key2);
}

static size_t pack(RpyBuf* buf) {
    return pack_thmodern(buf, &key1, &key2);
}

void rpy_th128(Rpy* rpy) {
    if (!rpy)
        return;
    rpy->unpack_fn = unpack;
    rpy->pack_fn = pack;
}
