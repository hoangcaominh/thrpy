#include "rpy.h"
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
