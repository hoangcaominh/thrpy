#include "msgutils.h"
#include <google/protobuf/util/json_util.h>
#include <google/protobuf/util/time_util.h>
#include <fstream>
#include <cstdio>
#include <string>
#include <ctime>
#include <sstream>

static time_t to_utc_time(const tm& tms) {
    tm utc = tms;
#ifdef _WIN32
    return _mkgmtime(&utc);
#else
    return timegm(&utc);
#endif
}

google::protobuf::Timestamp date_to_timestamp(const std::string& date, const char* fmt) {
    time_t now = std::time(nullptr);
    const tm* current = std::gmtime(&now);
    tm tms = {
        // Set default year to current year or year 2000 because of PCB
        .tm_year = current != nullptr ? current->tm_year : 100
    };
    std::istringstream iss(date);
    iss >> std::get_time(&tms, fmt);
    time_t time = to_utc_time(tms);

    return google::protobuf::util::TimeUtil::TimeTToTimestamp(time);
}


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
