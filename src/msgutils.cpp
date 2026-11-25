#include "msgutils.h"
#include <google/protobuf/util/json_util.h>
#include <fstream>
#include <cstdio>
#include <string>

bool msg_write(const google::protobuf::Message& msg, const char* outfile) {
    if (outfile == nullptr) {
        std::string json;
        auto status = google::protobuf::util::MessageToJsonString(msg, &json);
        if (!status.ok()) {
            std::string s{status.message()};
            fprintf(stderr, "%s\n", s.c_str());
            return false;
        }

        printf("%s\n", json.c_str());
        return true;
    }

    std::ofstream ofs(outfile, std::ios::binary | std::ios::trunc);
    if (!ofs) {
        fprintf(stderr, "Unable to open %s for writing.\n", outfile);
        return false;
    }
    if (!msg.SerializeToOstream(&ofs)) {
        fprintf(stderr, "Unable to write message to %s.\n", outfile);
        return false;
    }
    return true;
}
