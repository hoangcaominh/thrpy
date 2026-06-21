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

    RpyBuf buf;
    buf.reserve( DECOMPRESS_OFFSET + decompressed.size() + (data.size() - userdata_offset));
    buf.insert(buf.end(), data.begin(), data.begin() + DECRYPT_OFFSET);
    buf.insert(buf.end(), decrypted.begin(), decrypted.begin() + rel_offset);
    buf.insert(buf.end(), decompressed.begin(), decompressed.end());
    buf.insert(buf.end(), data.begin() + userdata_offset, data.end());

    return buf;
}

RpyBuf Rpy08::compile(const RpyBuf &data) {
    validate(data);

    uint32_t decomp_size = *(uint32_t*)(data.data() + 28);
    RpyBuf compressed = rpy_compress(data.begin() + DECOMPRESS_OFFSET, data.begin() + DECOMPRESS_OFFSET + decomp_size);
    uint32_t comp_size = compressed.size();
    
    RpyBuf buf;
    buf.reserve( data.size() + comp_size - decomp_size);
    buf.insert(buf.end(), data.begin() + DECRYPT_OFFSET, data.begin() + DECOMPRESS_OFFSET);
    buf.insert(buf.end(), compressed.begin(), compressed.end());
    memcpy(buf.data(), &comp_size, sizeof(comp_size));

    RpyBuf encrypred = rpy_encrypt06(buf.begin(), buf.end(), data[21]);

    buf.clear();
    buf.insert(buf.end(), data.begin(), data.begin() + DECRYPT_OFFSET);
    buf.insert(buf.end(), encrypred.begin(), encrypred.end());
    buf.insert(buf.end(), data.begin() + DECOMPRESS_OFFSET + decomp_size, data.end());

    return buf;
}

th08_t th08_deserialize(const RpyBuf& data) {
    std::string s(data.begin(), data.end());
    kaitai::kstream ks(s);
    th08_t instance(&ks);

    return instance;
}
