#pragma once

#include "rpy.h"

RpyBuf rpy_decrypt06(RpyBuf::const_iterator begin, RpyBuf::const_iterator end, uint8_t key);
RpyBuf rpy_encrypt06(RpyBuf::const_iterator begin, RpyBuf::const_iterator end, uint8_t key);
RpyBuf rpy_decrypt(RpyBuf::const_iterator begin, RpyBuf::const_iterator end, size_t block_size, uint8_t base, uint8_t add);
RpyBuf rpy_encrypt(RpyBuf::const_iterator begin, RpyBuf::const_iterator end, size_t block_size, uint8_t base, uint8_t add);
