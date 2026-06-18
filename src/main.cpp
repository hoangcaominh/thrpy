#include "thrpy.h"
#include <cstdio>
#include <cstring>
#include <optional>

int main(int argc, char* argv[]) {
    if (argc == 1) {
        printf("Usage: %s <replay>\n", argv[0]);
        return 0;
    }

    try {
        RpyBase* rpy = nullptr;
        const char* filename = argv[1];
        std::optional<RpyBuf> result = rpy_read(filename);
        if (!result) {
            fprintf(stderr, "Failed to read %s: %s\n", filename, strerror(errno));
            return 1;
        }
        RpyBuf buf = result.value();

        ThCode code = rpy_detect(buf);
        RpyBuf ret;
        const char* rpy_prefix;

        switch (code) {
            case ThCode::TH06:
                rpy = new Rpy06();
                rpy_prefix = "th6";
                break;
            case ThCode::TH07:
                rpy = new Rpy07();
                rpy_prefix = "th7";
                break;
            case ThCode::TH08:
                rpy = new Rpy08();
                rpy_prefix = "th8";
                break;
            default:
                break;
        }

        if (!rpy) {
            fprintf(stderr, "Unable to detect supported game\n");
            return 1;
        }

        std::string rpyname;

        RpyBuf dump = rpy->decompile(buf);
        rpyname = std::string(rpy_prefix) + "_uddump.rpy";
        rpy_write(dump, rpyname.c_str());

        RpyBuf dupe = rpy->compile(dump);
        rpyname = std::string(rpy_prefix) + "_uddupe.rpy";
        rpy_write(dupe, rpyname.c_str());
    } catch (std::exception& e) {
        fprintf(stderr, "Exception thrown: %s\n", e.what());
        return 1;
    }

    return 0;
}
