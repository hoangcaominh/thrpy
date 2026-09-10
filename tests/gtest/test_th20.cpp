#include "gtest/gtest.h"
#include "helper.h"
#include "config.h"
#include <cstring>

namespace {

class TestTh20 : public TestRpy {};

const char* REPLAYS[] = {
    "th20_01",   // main game
    "th20_02",   // extra
};
const size_t REPLAYS_SIZE = sizeof(REPLAYS) / sizeof(REPLAYS[0]);

TEST_F(TestTh20, DetectTh20) {
    for (size_t i = 0; i < REPLAYS_SIZE; i++) {
        snprintf(path, sizeof(path), PROJECT_DIR "/samples/th20/%s.rpy", REPLAYS[i]);
        rpybuf_read(buf, path);
        EXPECT_EQ(rpybuf_detect(buf), TH20);
    }
}

TEST_F(TestTh20, UnpackTh20) {
    rpy_th20(rpy);
    for (size_t i = 0; i < REPLAYS_SIZE; i++) {
        snprintf(path, sizeof(path), PROJECT_DIR "/samples/th20/%s.rpy", REPLAYS[i]);
        rpybuf_read(buf, path);
        snprintf(path, sizeof(path), PROJECT_DIR "/samples/th20/%s.rpx", REPLAYS[i]);
        rpybuf_read(expected_buf, path);
        rpy_unpack(rpy, buf, buf);
        EXPECT_EQ(buf->size, expected_buf->size);
        EXPECT_TRUE(memcmp(buf->data, expected_buf->data, buf->size) == 0);
        EXPECT_EQ(rpybuf_detect(buf), TH20);
    }
}

TEST_F(TestTh20, PackTh20) {
    rpy_th20(rpy);
    for (size_t i = 0; i < REPLAYS_SIZE; i++) {
        snprintf(path, sizeof(path), PROJECT_DIR "/samples/th20/%s.rpx", REPLAYS[i]);
        rpybuf_read(buf, path);
        snprintf(path, sizeof(path), PROJECT_DIR "/samples/th20/%s.rpy", REPLAYS[i]);
        rpybuf_read(expected_buf, path);
        rpy_pack(rpy, buf, buf);
        EXPECT_EQ(buf->size, expected_buf->size);
        EXPECT_TRUE(memcmp(buf->data, expected_buf->data, buf->size) == 0);
        EXPECT_EQ(rpybuf_detect(buf), TH20);
    }
}

}
