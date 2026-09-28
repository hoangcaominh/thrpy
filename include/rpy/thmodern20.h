#pragma once

#include "rpy.h"
#include "crypt.h"

#ifdef __cplusplus
extern "C" {
#endif

size_t unpack_thmodern20(RpyBuf* buf, RpyModernKey* key1, RpyModernKey* key2);
size_t pack_thmodern20(RpyBuf* buf, RpyModernKey* key1, RpyModernKey* key2);

#ifdef __cplusplus
}
#endif
