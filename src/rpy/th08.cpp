#include "rpy/th08.h"
#include "crypt.h"
#include "lzss.h"
#include <cstring>
#include <stdexcept>

constexpr size_t DECRYPT_OFFSET = 24;
constexpr size_t DECOMPRESS_OFFSET = 104;

static inline void validate(const RpyBuf& data) {
    if (data.size() < DECOMPRESS_OFFSET)
        throw std::runtime_error("Invalid data");
}

RpyBuf Rpy08::decompile(const RpyBuf &data) {
    validate(data);

    uint32_t userdata_offset = *(uint32_t*)(data.data() + 12);
    RpyBuf decrypted = rpy_decrypt06(data.begin() + DECRYPT_OFFSET, data.begin() + userdata_offset, data[21]);

    size_t rel_offset = DECOMPRESS_OFFSET - DECRYPT_OFFSET;
    RpyBuf decompressed = rpy_decompress(decrypted.begin() + rel_offset, decrypted.end());

    RpyBuf ret;
    ret.insert(ret.end(), data.begin(), data.begin() + DECRYPT_OFFSET);
    ret.insert(ret.end(), decrypted.begin(), decrypted.begin() + rel_offset);
    ret.insert(ret.end(), decompressed.begin(), decompressed.end());
    ret.insert(ret.end(), data.begin() + userdata_offset, data.end());

    return ret;
}

RpyBuf Rpy08::compile(const RpyBuf &data) {
    validate(data);

    uint32_t decomp_size = *(uint32_t*)(data.data() + 28);
    RpyBuf compressed = rpy_compress(data.begin() + DECOMPRESS_OFFSET, data.begin() + DECOMPRESS_OFFSET + decomp_size);
    uint32_t comp_data_size = compressed.size();
    
    RpyBuf buf;
    buf.insert(buf.end(), data.begin() + DECRYPT_OFFSET, data.begin() + DECOMPRESS_OFFSET);
    buf.insert(buf.end(), compressed.begin(), compressed.end());
    memcpy(buf.data(), &comp_data_size, sizeof(comp_data_size));

    RpyBuf encrypred = rpy_encrypt06(buf.begin(), buf.end(), data[21]);

    RpyBuf ret;
    ret.insert(ret.end(), data.begin(), data.begin() + DECRYPT_OFFSET);
    ret.insert(ret.end(), encrypred.begin(), encrypred.end());
    ret.insert(ret.end(), data.begin() + DECOMPRESS_OFFSET + decomp_size, data.end());

    return ret;
}

th08_t th08_deserialize(const RpyBuf& data) {
    std::string s(data.begin(), data.end());
    kaitai::kstream ks(s);
    th08_t instance(&ks);

    return instance;
}
