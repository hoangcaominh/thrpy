#include <gtest/gtest.h>
#include "thrpy.h"
#include "config.h"

namespace {

TEST(TestDetect, DetectTh06) {
    RpyBuf* buf = rpybuf_init();

    rpybuf_read(buf, PROJECT_DIR "/samples/th06/th6_01.rpy");
    EXPECT_EQ(rpybuf_detect(buf), TH06);

    rpybuf_read(buf, PROJECT_DIR "/samples/th06/th6_02.rpy");
    EXPECT_EQ(rpybuf_detect(buf), TH06);

    rpybuf_destroy(buf);
}

}
