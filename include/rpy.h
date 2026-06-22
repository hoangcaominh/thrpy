#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

struct rpybuf {
    uint8_t* data;
    size_t size;
    size_t capacity;
};
typedef struct rpybuf RpyBuf;

typedef struct rpy Rpy;
struct rpy {
    void (*unpack)(const RpyBuf*, RpyBuf*);
    void (*pack)(const RpyBuf*, RpyBuf*);
};

enum thcode {
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
typedef enum thcode ThCode;

// const char* const TH_CODE_STR_TABLE[] = {
//     "",
//     "th06",
//     "th07",
//     "th08",
//     "th09",
//     "th10",
//     "th11",
//     "th12",
//     "th128",
//     "th13",
//     "th14",
//     "th15",
//     "th16",
//     "th17",
//     "th18",
//     "th20",
// };

// const size_t TH_CODE_STR_TABLE_LEN = sizeof(TH_CODE_STR_TABLE) / sizeof(*TH_CODE_STR_TABLE);

#ifdef __cplusplus
extern "C" {
#endif

Rpy* rpy_init();
void rpy_destroy(Rpy* rpy);

RpyBuf* rpybuf_init();
size_t rpybuf_read(RpyBuf* buf, const char* file);
size_t rpybuf_write(const RpyBuf* buf, const char* file);
void rpybuf_destroy(RpyBuf* buf);

#ifdef __cplusplus
}
#endif
