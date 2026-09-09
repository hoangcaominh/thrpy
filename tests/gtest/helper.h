#pragma once

#include <gtest/gtest.h>
#include "thrpy.h"

class TestRpy : public testing::Test {
public:
    virtual void SetUp() override {
        rpy = rpy_init();
        buf = rpybuf_init();
        expected_buf = rpybuf_init();
    }

    virtual void TearDown() override {
        rpybuf_destroy(buf);
        rpybuf_destroy(expected_buf);
        rpy_destroy(rpy);
    }

    RpyBuf* buf;
    RpyBuf* expected_buf;
    Rpy* rpy;
    char path[256];
};
