#include "crypt.h"
#include "impl/crypt_impl.h"

RpyBuf rpy_decrypt06(RpyBuf::const_iterator begin, RpyBuf::const_iterator end, uint8_t key) {
	RpyBuf ret(begin, end);
    decrypt06(ret.data(), ret.size(), key);
	return ret;
}

RpyBuf rpy_encrypt06(RpyBuf::const_iterator begin, RpyBuf::const_iterator end, uint8_t key) {
	RpyBuf ret(begin, end);
    encrypt06(ret.data(), ret.size(), key);
	return ret;
}

RpyBuf rpy_decrypt(RpyBuf::const_iterator begin, RpyBuf::const_iterator end, size_t block_size, uint8_t base, uint8_t add) {
	RpyBuf ret(begin, end);
    decrypt(ret.data(), ret.size(), block_size, base, add);
	return ret;
}

RpyBuf rpy_encrypt(RpyBuf::const_iterator begin, RpyBuf::const_iterator end, size_t block_size, uint8_t base, uint8_t add) {
	RpyBuf ret(begin, end);
    encrypt(ret.data(), ret.size(), block_size, base, add);
	return ret;
}
