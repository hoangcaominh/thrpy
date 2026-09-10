#include "rpy/th10.h"
#include "thmodern.h"

static RpyModernKey key1 = { 0x400, 0xAA, 0xE1 };
static RpyModernKey key2 = { 0x80, 0x3D, 0x7A };

static size_t unpack(RpyBuf* buf) {
    return unpack_thmodern(buf, &key1, &key2);
}

static size_t pack(RpyBuf* buf) {
    return pack_thmodern(buf, &key1, &key2);
}

void rpy_th10(Rpy* rpy) {
    if (!rpy)
        return;
    rpy->gamecode = TH10;
    rpy->unpack_fn = unpack;
    rpy->pack_fn = pack;
}
