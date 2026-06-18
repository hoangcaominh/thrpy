#include "rpy.h"
#include "ksy/th_modern.h"
#include <cstdio>
#include <cstring>
#include <fstream>
#include <sstream>

std::optional<RpyBuf> rpy_read(const char* file) {
    std::ifstream ifs(file, std::ios_base::binary);
    if (ifs.fail())
        return {};

    std::ostringstream oss;
    oss << ifs.rdbuf();
    const std::string& s = oss.str();
    RpyBuf data(s.begin(), s.end());
    return data;
}

bool rpy_write(const RpyBuf& buf, const char* file) {
    std::ofstream ofs(file, std::ios_base::binary);
    if (ofs.fail())
        return false;

    ofs.write(reinterpret_cast<const char*>(buf.data()), buf.size());
    return true;
}

ThCode rpy_detect_th13_or_th14(const RpyBuf& buf) {
    std::string s(buf.begin(), buf.end());
    kaitai::kstream ks(s);
    th_modern_t header(&ks);

    uint8_t byte = header.userdata()->user_desc()->at(4);
    // SJIS character for 廟
    if (byte == 0x90 || byte == 0xC9)
        return ThCode::TH13;
    // SJIS character for 城
    else if (byte == 0x8B || byte == 0xBB)
        return ThCode::TH14;
    // Rip replay
    return ThCode::THNA;
}

ThCode rpy_detect(const RpyBuf& buf) {
    if (buf.size() < 4)
        return ThCode::THNA;
    uint32_t magic = *(uint32_t*)buf.data();
    switch (magic) {
        case 0x50523654:    // T6RP
            return ThCode::TH06;
        case 0x50523754:    // T7RP
            return ThCode::TH07;
        case 0x50523854:    // T8RP
            return ThCode::TH08;
        case 0x50523954:    // T9RP
            return ThCode::TH09;
        case 0x72303174:    // t10r
            return ThCode::TH10;
        case 0x72313174:    // t11r
            return ThCode::TH11;
        case 0x72323174:    // t12r
            return ThCode::TH12;
        case 0x72383231:    // 128r
            return ThCode::TH128;
        case 0x72333174:    // t13r
            return rpy_detect_th13_or_th14(buf);
        case 0x72353174:    // t15r
            return ThCode::TH15;
        case 0x72363174:    // t16r
            return ThCode::TH16;
        case 0x72373174:    // t17r
            return ThCode::TH17;
        case 0x72383174:    // t18r
            return ThCode::TH18;
        case 0x72303274:    // t20r
            return ThCode::TH20;
        default:
            return ThCode::THNA;
    }
}
