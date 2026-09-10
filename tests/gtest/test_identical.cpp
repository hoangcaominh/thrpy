#include "gtest/gtest.h"
#include "helper.h"
#include "config.h"
#include <cstring>

namespace {

const char* REPLAYS[] = {
    "samples/th06/th6_01.rpy",
    "samples/th06/th6_02.rpy",
    "samples/th07/th7_01.rpy",
    "samples/th07/th7_02.rpy",
    "samples/th07/th7_03.rpy",
    "samples/th08/th8_01.rpy",
    "samples/th08/th8_02.rpy",
    "samples/th08/th8_03.rpy",
    "samples/th08/th8_04.rpy",
    "samples/th08/th8_05.rpy",
    "samples/th10/th10_01.rpy",
    "samples/th10/th10_02.rpy",
    "samples/th11/th11_01.rpy",
    "samples/th11/th11_02.rpy",
    "samples/th12/th12_01.rpy",
    "samples/th12/th12_02.rpy",
    "samples/th128/th128_01.rpy",
    "samples/th128/th128_02.rpy",
    "samples/th128/th128_03.rpy",
    "samples/th128/th128_04.rpy",
    "samples/th128/th128_05.rpy",
    "samples/th128/th128_06.rpy",
    "samples/th128/th128_07.rpy",
    "samples/th13/th13_01.rpy",
    "samples/th13/th13_02.rpy",
    "samples/th13/th13_03.rpy",
    "samples/th14/th14_01.rpy",
    "samples/th14/th14_02.rpy",
    "samples/th14/th14_03.rpy",
    "samples/th15/th15_01.rpy",
    "samples/th15/th15_02.rpy",
    "samples/th16/th16_01.rpy",
    "samples/th16/th16_02.rpy",
    "samples/th17/th17_01.rpy",
    "samples/th17/th17_02.rpy",
    "samples/th17/th17_03.rpy",
    "samples/th18/th18_01.rpy",
    "samples/th18/th18_02.rpy",
    "samples/th20/th20_01.rpy",
    "samples/th20/th20_02.rpy",
};
const size_t REPLAYS_SIZE = sizeof(REPLAYS) / sizeof(REPLAYS[0]);

TEST_F(TestRpy, Identical) {
    for (size_t i = 0; i < REPLAYS_SIZE; i++) {
        snprintf(path, sizeof(path), PROJECT_DIR "/%s", REPLAYS[i]);
        rpybuf_read(buf, path);
        rpybuf_read(expected_buf, path);
        rpy_autoconf(rpy, buf);
        rpy_unpack(rpy, buf, buf);
        // Debug
        rpy_pack(rpy, buf, buf);

        EXPECT_EQ(buf->size, expected_buf->size);
        EXPECT_TRUE(memcmp(buf->data, expected_buf->data, buf->size) == 0);
    }
}

}
