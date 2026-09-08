#include "gtest/gtest.h"
#include "thrpy.h"
#include "config.h"

namespace {

class TestDetect : public testing::Test {
protected:
    void SetUp() override {
        buf = rpybuf_init();
    }

    void TearDown() override {
        rpybuf_destroy(buf);
    }

    size_t get_replay(const char* path) {
        return rpybuf_read(buf, path);
    }

    RpyBuf* buf;
};

TEST_F(TestDetect, DetectTh06_1) {
    get_replay(PROJECT_DIR "/samples/th06/th6_01.rpy");
    EXPECT_EQ(rpybuf_detect(buf), TH06);
}

TEST_F(TestDetect, DetectTh06_2) {
    get_replay(PROJECT_DIR "/samples/th06/th6_02.rpy");
    EXPECT_EQ(rpybuf_detect(buf), TH06);
}

}
