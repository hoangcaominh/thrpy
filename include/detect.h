#pragma once

#include "rpy.h"

#ifdef __cplusplus
extern "C" {
#endif

ThCode rpybuf_detect(const RpyBuf* buf);
void rpy_autoconf(Rpy* rpy, const RpyBuf* buf);

#ifdef __cplusplus
}
#endif

