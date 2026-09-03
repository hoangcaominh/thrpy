#include "rpy/th15.h"
#include "thmodern.h"

static RpyModernKey key1 = { 0x400, 0x5C, 0xE1 };
static RpyModernKey key2 = { 0x100, 0x7D, 0x3A };

static size_t unpack(RpyBuf* buf) {
    return unpack_thmodern(buf, &key1, &key2);
}

static size_t pack(RpyBuf* buf) {
    return pack_thmodern(buf, &key1, &key2);
}

void rpy_th15(Rpy* rpy) {
    if (!rpy)
        return;
    rpy->unpack_fn = unpack;
    rpy->pack_fn = pack;
}
