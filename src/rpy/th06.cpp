#include "rpy/th06.h"
#include "crypt.h"
#include <stdexcept>

constexpr size_t DECRYPT_OFFSET = 15;

static inline void validate(const RpyBuf& data) {
    if (data.size() < DECRYPT_OFFSET)
        throw std::runtime_error("Invalid data");
}

RpyBuf Rpy06::decompile(const RpyBuf& data) {
    validate(data);

    RpyBuf decryped = rpy_decrypt06(data.begin() + DECRYPT_OFFSET, data.end(), data[14]);

    RpyBuf buf;
    buf.reserve(DECRYPT_OFFSET + decryped.size());
    buf.insert(buf.end(), data.begin(), data.begin() + DECRYPT_OFFSET);
    buf.insert(buf.end(), decryped.begin(), decryped.end());

    return buf;
}

RpyBuf Rpy06::compile(const RpyBuf& data) {
    validate(data);

    RpyBuf encrypted = rpy_encrypt06(data.begin() + DECRYPT_OFFSET, data.end(), data[14]);

    RpyBuf buf;
    buf.reserve(DECRYPT_OFFSET + encrypted.size());
    buf.insert(buf.end(), data.begin(), data.begin() + DECRYPT_OFFSET);
    buf.insert(buf.end(), encrypted.begin(), encrypted.end());

    return buf;
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
