#include "rpy/th07.h"
#include "crypt.h"
#include "lzss.h"
#include <cstring>

RpyBuf Rpy07::decompile(const RpyBuf& data) {
    RpyBuf decrypted = rpy_decrypt06(data.begin() + 16, data.end(), data[13]);

    uint32_t comp_data_size = *(uint32_t*)(decrypted.data() + 4);
    RpyBuf decompressed = rpy_decompress(decrypted.begin() + 68, decrypted.end());

    RpyBuf ret;
    ret.insert(ret.end(), data.begin(), data.begin() + 16);
    ret.insert(ret.end(), decrypted.begin(), decrypted.begin() + 68);
    ret.insert(ret.end(), decompressed.begin(), decompressed.end());

    return ret;
}

RpyBuf Rpy07::compile(const RpyBuf& data) {
    RpyBuf compressed = rpy_compress(data.begin() + 84, data.end());
    uint32_t comp_data_size = compressed.size();
    
    RpyBuf buf;
    buf.insert(buf.end(), data.begin() + 16, data.begin() + 84);
    buf.insert(buf.end(), compressed.begin(), compressed.end());
    memcpy(buf.data() + 4, &comp_data_size, sizeof(comp_data_size));

    RpyBuf encrypred = rpy_encrypt06(buf.begin(), buf.end(), data[13]);

    RpyBuf ret;
    ret.insert(ret.end(), data.begin(), data.begin() + 16);
    ret.insert(ret.end(), encrypred.begin(), encrypred.end());

    return ret;
}

th07_t th07_deserialize(const RpyBuf& data) {
    std::string s(data.begin(), data.end());
    kaitai::kstream ks(s);
    th07_t instance(&ks);

    return instance;
}
