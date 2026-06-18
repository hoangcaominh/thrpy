#include "rpy/th06.h"
#include "kaitai/kaitaistream.h"
#include "crypt.h"

RpyBuf Rpy06::decompile(const RpyBuf& data) {
    RpyBuf decryped = rpy_decrypt06(data.begin() + 15, data.end(), data[14]);

    RpyBuf ret;
    ret.insert(ret.end(), data.begin(), data.begin() + 15);
    ret.insert(ret.end(), decryped.begin(), decryped.end());

    return ret;
}

RpyBuf Rpy06::compile(const RpyBuf& data) {
    RpyBuf encrypted = rpy_encrypt06(data.begin() + 15, data.end(), data[14]);

    RpyBuf ret;
    ret.insert(ret.end(), data.begin(), data.begin() + 15);
    ret.insert(ret.end(), encrypted.begin(), encrypted.end());

    return ret;
}

th06_t th06_deserialize(const RpyBuf &data) {
    std::string s(data.begin(), data.end());
    kaitai::kstream ks(s);
    th06_t instance(&ks);

    return instance;
}

RpyBuf th06_serialize(const th06_t &data) {
    return RpyBuf();
}
