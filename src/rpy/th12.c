#include "rpy/th12.h"
#include "thmodern.h"

static RpyModernKey key1 = { 0x800, 0x5E, 0xE1 };
static RpyModernKey key2 = { 0x40, 0x7D, 0x3A };

static size_t unpack(RpyBuf* buf) {
    return unpack_thmodern(buf, &key1, &key2);
}

static size_t pack(RpyBuf* buf) {
    return pack_thmodern(buf, &key1, &key2);
}

void rpy_th12(Rpy* rpy) {
    if (!rpy)
        return;
    rpy->gamecode = TH12;
    rpy->unpack_fn = unpack;
    rpy->pack_fn = pack;
}
