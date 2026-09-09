#include "gtest/gtest.h"
#include "helper.h"
#include "config.h"
#include <cstring>

namespace {

class TestTh08 : public TestRpy {};

const char* REPLAYS[] = {
    "th8_01",   // main game A
    "th8_02",   // main game B
    "th8_03",   // extra
    "th8_04",   // stage practice
    "th8_05",   // spell practice - last word
};
const size_t REPLAYS_SIZE = sizeof(REPLAYS) / sizeof(REPLAYS[0]);

TEST_F(TestTh08, DetectTh08) {
    for (size_t i = 0; i < REPLAYS_SIZE; i++) {
        snprintf(path, sizeof(path), PROJECT_DIR "/samples/th08/%s.rpy", REPLAYS[i]);
        rpybuf_read(buf, path);
        EXPECT_EQ(rpybuf_detect(buf), TH08);
    }
}

TEST_F(TestTh08, UnpackTh08) {
    rpy_th08(rpy);
    for (size_t i = 0; i < REPLAYS_SIZE; i++) {
        snprintf(path, sizeof(path), PROJECT_DIR "/samples/th08/%s.rpy", REPLAYS[i]);
        rpybuf_read(buf, path);
        snprintf(path, sizeof(path), PROJECT_DIR "/samples/th08/%s.rpx", REPLAYS[i]);
        rpybuf_read(expected_buf, path);
        rpy_unpack(rpy, buf, buf);
        EXPECT_EQ(buf->size, expected_buf->size);
        EXPECT_TRUE(memcmp(buf->data, expected_buf->data, buf->size) == 0);
        EXPECT_EQ(rpybuf_detect(buf), TH08);
    }
}

TEST_F(TestTh08, PackTh08) {
    rpy_th08(rpy);
    for (size_t i = 0; i < REPLAYS_SIZE; i++) {
        snprintf(path, sizeof(path), PROJECT_DIR "/samples/th08/%s.rpx", REPLAYS[i]);
        rpybuf_read(buf, path);
        snprintf(path, sizeof(path), PROJECT_DIR "/samples/th08/%s.rpy", REPLAYS[i]);
        rpybuf_read(expected_buf, path);
        rpy_pack(rpy, buf, buf);
        EXPECT_EQ(buf->size, expected_buf->size);
        EXPECT_TRUE(memcmp(buf->data, expected_buf->data, buf->size) == 0);
        EXPECT_EQ(rpybuf_detect(buf), TH08);
    }
}

}
