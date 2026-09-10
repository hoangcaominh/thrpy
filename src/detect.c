#include "detect.h"
#include "rpy/th06.h"
#include "rpy/th07.h"
#include "rpy/th08.h"
#include "rpy/th10.h"
#include "rpy/th11.h"
#include "rpy/th12.h"
#include "rpy/th128.h"
#include "rpy/th13.h"
#include "rpy/th14.h"
#include "rpy/th15.h"
#include "rpy/th16.h"
#include "rpy/th17.h"
#include "rpy/th18.h"
#include "rpy/th20.h"

ThCode rpybuf_detect_th13_or_th14(const RpyBuf* buf) {
    if (buf->size < 28)
        return THNA;

    // comp_data_offset + comp_data_size + user_desc_offset + 4
    size_t offset = 36 + *(uint32_t*)(buf->data + 28) + 12 + 4;
    if (buf->size < offset)
        return THNA;

    uint8_t byte = buf->data[offset];
    // SJIS character for 廟
    if (byte == 0x90 || byte == 0xC9)
        return TH13;
    // SJIS character for 城
    else if (byte == 0x8B || byte == 0xBB)
        return TH14;
    // Rip replay
    return THNA;
}

ThCode rpybuf_detect(const RpyBuf* buf) {
    if (buf->size < 4)
        return THNA;
    uint32_t magic = *(uint32_t*)buf->data;
    switch (magic) {
        case 0x50523654:    // T6RP
            return TH06;
        case 0x50523754:    // T7RP
            return TH07;
        case 0x50523854:    // T8RP
            return TH08;
        case 0x50523954:    // T9RP
            return TH09;
        case 0x72303174:    // t10r
            return TH10;
        case 0x72313174:    // t11r
            return TH11;
        case 0x72323174:    // t12r
            return TH12;
        case 0x72383231:    // 128r
            return TH128;
        case 0x72333174:    // t13r
            return rpybuf_detect_th13_or_th14(buf);
        case 0x72353174:    // t15r
            return TH15;
        case 0x72363174:    // t16r
            return TH16;
        case 0x72373174:    // t17r
            return TH17;
        case 0x72383174:    // t18r
            return TH18;
        case 0x72303274:    // t20r
            return TH20;
        default:
            return THNA;
    }
}

void rpy_autoconf(Rpy* rpy, const RpyBuf* buf) {
    switch (rpybuf_detect(buf)) {
        case TH06:
            rpy_th06(rpy);
            break;
        case TH07:
            rpy_th07(rpy);
            break;
        case TH08:
            rpy_th08(rpy);
            break;
        case TH10:
            rpy_th10(rpy);
            break;
        case TH11:
            rpy_th11(rpy);
            break;
        case TH12:
            rpy_th12(rpy);
            break;
        case TH128:
            rpy_th128(rpy);
            break;
        case TH13:
            rpy_th13(rpy);
            break;
        case TH14:
            rpy_th14(rpy);
            break;
        case TH15:
            rpy_th15(rpy);
            break;
        case TH16:
            rpy_th16(rpy);
            break;
        case TH17:
            rpy_th17(rpy);
            break;
        case TH18:
            rpy_th18(rpy);
            break;
        case TH20:
            rpy_th20(rpy);
            break;
        default:
            break;
    }
}
