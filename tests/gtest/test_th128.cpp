#include "gtest/gtest.h"
#include "helper.h"
#include "config.h"
#include <cstring>

namespace {

class TestTh128 : public TestRpy {};

const char* REPLAYS[] = {
    "th128_01.rpy",   // normal a1
    "th128_02.rpy",   // hard a2
    "th128_03.rpy",   // lunatic b1
    "th128_04.rpy",   // normal b2
    "th128_05.rpy",   // hard c1
    "th128_06.rpy",   // lunatic c3
    "th128_07.rpy",   // extra
};
const size_t REPLAYS_SIZE = sizeof(REPLAYS) / sizeof(REPLAYS[0]);

TEST_F(TestTh128, DetectTh128) {
    for (size_t i = 0; i < REPLAYS_SIZE; i++) {
        snprintf(path, sizeof(path), PROJECT_DIR "/samples/th128/%s", REPLAYS[i]);
        rpybuf_read(buf, path);
        EXPECT_EQ(rpybuf_detect(buf), TH128);
    }
}

TEST_F(TestTh128, UnpackTh128) {
    rpy_th128(rpy);
    for (size_t i = 0; i < REPLAYS_SIZE; i++) {
        snprintf(path, sizeof(path), PROJECT_DIR "/samples/th128/%s", REPLAYS[i]);
        rpybuf_read(buf, path);
        snprintf(path, sizeof(path), PROJECT_DIR "/samples/th128/%s.raw", REPLAYS[i]);
        rpybuf_read(expected_buf, path);
        rpy_unpack(rpy, buf, buf);
        EXPECT_EQ(buf->size, expected_buf->size);
        EXPECT_TRUE(memcmp(buf->data, expected_buf->data, buf->size) == 0);
    }
}

TEST_F(TestTh128, PackTh128) {
    rpy_th128(rpy);
    for (size_t i = 0; i < REPLAYS_SIZE; i++) {
        snprintf(path, sizeof(path), PROJECT_DIR "/samples/th128/%s.raw", REPLAYS[i]);
        rpybuf_read(buf, path);
        snprintf(path, sizeof(path), PROJECT_DIR "/samples/th128/%s", REPLAYS[i]);
        rpybuf_read(expected_buf, path);
        rpy_pack(rpy, buf, buf);
        EXPECT_EQ(buf->size, expected_buf->size);
        EXPECT_TRUE(memcmp(buf->data, expected_buf->data, buf->size) == 0);
    }
}

}
