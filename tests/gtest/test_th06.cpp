#include "gtest/gtest.h"
#include "helper.h"
#include "config.h"
#include <string.h>

namespace {

class TestTh06 : public TestRpy {};

TEST_F(TestTh06, DetectTh06_1) {
    rpybuf_read(buf, PROJECT_DIR "/samples/th06/th6_01.rpy");
    EXPECT_EQ(rpybuf_detect(buf), TH06);
}

TEST_F(TestTh06, DetectTh06_2) {
    rpybuf_read(buf, PROJECT_DIR "/samples/th06/th6_02.rpy");
    EXPECT_EQ(rpybuf_detect(buf), TH06);
}

TEST_F(TestTh06, UnpackTh06_1) {
    rpy_th06(rpy);
    rpybuf_read(buf, PROJECT_DIR "/samples/th06/th6_01.rpy");
    rpybuf_read(expected_buf, PROJECT_DIR "/samples/th06/th6_01.rpx");
    rpy_unpack(rpy, buf, buf);
    EXPECT_EQ(buf->size, expected_buf->size);
    EXPECT_TRUE(memcmp(buf->data, expected_buf->data, buf->size) == 0);
}

TEST_F(TestTh06, UnpackTh06_2) {
    rpy_th06(rpy);
    rpybuf_read(buf, PROJECT_DIR "/samples/th06/th6_02.rpy");
    rpybuf_read(expected_buf, PROJECT_DIR "/samples/th06/th6_02.rpx");
    rpy_unpack(rpy, buf, buf);
    EXPECT_EQ(buf->size, expected_buf->size);
    EXPECT_TRUE(memcmp(buf->data, expected_buf->data, buf->size) == 0);
}

TEST_F(TestTh06, PackTh06_1) {
    rpy_th06(rpy);
    rpybuf_read(buf, PROJECT_DIR "/samples/th06/th6_01.rpx");
    rpybuf_read(expected_buf, PROJECT_DIR "/samples/th06/th6_01.rpy");
    rpy_pack(rpy, buf, buf);
    EXPECT_EQ(buf->size, expected_buf->size);
    EXPECT_TRUE(memcmp(buf->data, expected_buf->data, buf->size) == 0);
}

TEST_F(TestTh06, PackTh06_2) {
    rpy_th06(rpy);
    rpybuf_read(buf, PROJECT_DIR "/samples/th06/th6_02.rpx");
    rpybuf_read(expected_buf, PROJECT_DIR "/samples/th06/th6_02.rpy");
    rpy_pack(rpy, buf, buf);
    EXPECT_EQ(buf->size, expected_buf->size);
    EXPECT_TRUE(memcmp(buf->data, expected_buf->data, buf->size) == 0);
}

}
