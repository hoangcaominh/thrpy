#pragma once

#include <cstdint>
#include <optional>
#include <vector>

typedef std::vector<uint8_t> RpyBuf;

enum class ThCode {
    THNA,
    TH06,
    TH07,
    TH08,
    TH09,
    TH10,
    TH11,
    TH12,
    TH128,
    TH13,
    TH14,
    TH15,
    TH16,
    TH17,
    TH18,
    TH20,
};

class RpyBase {
public:
    virtual ~RpyBase() = default;

    virtual RpyBuf decompile(const RpyBuf& data) = 0;
    virtual RpyBuf compile(const RpyBuf& data) = 0;
};

const char* const TH_CODE_STR_TABLE[] = {
    "",
    "th06",
    "th07",
    "th08",
    "th09",
    "th10",
    "th11",
    "th12",
    "th128",
    "th13",
    "th14",
    "th15",
    "th16",
    "th17",
    "th18",
    "th20",
};

const size_t TH_CODE_STR_TABLE_LEN = sizeof(TH_CODE_STR_TABLE) / sizeof(*TH_CODE_STR_TABLE);

std::optional<RpyBuf> rpy_read(const char* file);
bool rpy_write(const RpyBuf& buf, const char* file);
ThCode rpy_detect(const RpyBuf& buf);
