#include "msgutils.h"
#include <google/protobuf/util/json_util.h>
#include <fstream>
#include <print>
#include <string>

bool msg_write(const google::protobuf::Message& msg, const char* outfile) {
    if (outfile == nullptr) {
        std::string json;
        auto status = google::protobuf::util::MessageToJsonString(msg, &json);
        if (!status.ok()) {
            std::println(stderr, "{}", status.message());
            return false;
        }

        std::println("{}", json);
        return true;
    }

    std::ofstream ofs(outfile, std::ios::binary | std::ios::trunc);
    if (!ofs) {
        std::println(stderr, "Unable to open {} for writing.", outfile);
        return false;
    }
    if (!msg.SerializeToOstream(&ofs)) {
        std::println(stderr, "Unable to write message to {}.", outfile);
        return false;
    }
    return true;
}
