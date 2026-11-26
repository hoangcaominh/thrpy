#include "gtest/gtest.h"
#include "helper.h"
#include "config.h"
#include <cstring>

namespace {

class TestTh06nc : public TestRpy {};

const char* REPLAYS[] = {
    "th6_01.rpy",   // main game
    "th6_02.rpy",   // extra
    "th6_03.rpy",   // challenge
    "th6_04.rpy",   // spell practice
};
const size_t REPLAYS_SIZE = sizeof(REPLAYS) / sizeof(REPLAYS[0]);

TEST_F(TestTh06nc, DetectTh06nc) {
    for (size_t i = 0; i < REPLAYS_SIZE; i++) {
        snprintf(path, sizeof(path), PROJECT_DIR "/samples/th06nc/%s", REPLAYS[i]);
        rpybuf_read(buf, path);
        EXPECT_EQ(rpybuf_detect(buf), TH06);
    }
}

TEST_F(TestTh06nc, UnpackTh06nc) {
    rpy_th06(rpy);
    for (size_t i = 0; i < REPLAYS_SIZE; i++) {
        snprintf(path, sizeof(path), PROJECT_DIR "/samples/th06nc/%s", REPLAYS[i]);
        rpybuf_read(buf, path);
        snprintf(path, sizeof(path), PROJECT_DIR "/samples/th06nc/%s.raw", REPLAYS[i]);
        rpybuf_read(expected_buf, path);
        rpy_unpack(rpy, buf, buf);
        EXPECT_EQ(buf->size, expected_buf->size);
        EXPECT_TRUE(memcmp(buf->data, expected_buf->data, buf->size) == 0);
    }
}

TEST_F(TestTh06nc, PackTh06nc) {
    rpy_th06(rpy);
    for (size_t i = 0; i < REPLAYS_SIZE; i++) {
        snprintf(path, sizeof(path), PROJECT_DIR "/samples/th06nc/%s.raw", REPLAYS[i]);
        rpybuf_read(buf, path);
        snprintf(path, sizeof(path), PROJECT_DIR "/samples/th06nc/%s", REPLAYS[i]);
        rpybuf_read(expected_buf, path);
        rpy_pack(rpy, buf, buf);
        EXPECT_EQ(buf->size, expected_buf->size);
        EXPECT_TRUE(memcmp(buf->data, expected_buf->data, buf->size) == 0);
    }
}

}
