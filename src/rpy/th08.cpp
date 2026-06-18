#include "rpy/th08.h"
#include "crypt.h"
#include "lzss.h"
#include <cstring>

RpyBuf Rpy08::decompile(const RpyBuf &buf) {
    uint32_t userdata_offset = *(uint32_t*)(buf.data() + 12);
    RpyBuf decrypted = rpy_decrypt06(buf.begin() + 24, buf.begin() + userdata_offset, buf[21]);

    RpyBuf decompressed = rpy_decompress(decrypted.begin() + 80, decrypted.end());

    RpyBuf ret;
    ret.insert(ret.end(), buf.begin(), buf.begin() + 24);
    ret.insert(ret.end(), decrypted.begin(), decrypted.begin() + 80);
    ret.insert(ret.end(), decompressed.begin(), decompressed.end());
    ret.insert(ret.end(), buf.begin() + userdata_offset, buf.end());

    return ret;
}

RpyBuf Rpy08::compile(const RpyBuf &data) {
    uint32_t decomp_size = *(uint32_t*)(data.data() + 28);
    RpyBuf compressed = rpy_compress(data.begin() + 104, data.begin() + 104 + decomp_size);
    uint32_t comp_data_size = compressed.size();
    
    RpyBuf buf;
    buf.insert(buf.end(), data.begin() + 24, data.begin() + 104);
    buf.insert(buf.end(), compressed.begin(), compressed.end());
    memcpy(buf.data(), &comp_data_size, sizeof(comp_data_size));

    RpyBuf encrypred = rpy_encrypt06(buf.begin(), buf.end(), data[21]);

    RpyBuf ret;
    ret.insert(ret.end(), data.begin(), data.begin() + 24);
    ret.insert(ret.end(), encrypred.begin(), encrypred.end());
    ret.insert(ret.end(), data.begin() + 104 + decomp_size, data.end());

    return ret;
}

th08_t th08_deserialize(const RpyBuf& buf) {
    std::string s(buf.begin(), buf.end());
    kaitai::kstream ks(s);
    th08_t instance(&ks);

    return instance;
}
