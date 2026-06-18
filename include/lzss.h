#pragma once

#include "rpy.h"

RpyBuf rpy_decompress(RpyBuf::const_iterator begin, RpyBuf::const_iterator end);
RpyBuf rpy_compress(RpyBuf::const_iterator begin, RpyBuf::const_iterator end);
