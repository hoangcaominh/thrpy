#include "thrpy.h"
#include <stdio.h>

int main(int argc, char* argv[]) {
    if (argc == 1) {
        printf("Usage: %s <replay>\n", argv[0]);
        return 0;
    }

    Rpy* rpy;
    RpyBuf* buf;
    char rpyname[100];
    const char* rpy_prefix;
    const char* filename = argv[1];
    int status = 0;

    rpy = rpy_init();
    if (!rpy)
        goto ret_fail;

    buf = rpybuf_init();
    if (!buf)
        goto ret_fail;

    if (rpybuf_read(buf, filename) == 0)
        goto ret_fail;

    switch (rpybuf_detect(buf)) {
        case TH06:
            rpy_th06(rpy);
            rpy_prefix = "th6";
            break;
        case TH07:
            rpy_th07(rpy);
            rpy_prefix = "th7";
            break;
        case TH08:
            rpy_th08(rpy);
            rpy_prefix = "th8";
            break;
        default:
            fprintf(stderr, "Unable to detect supported game\n");
            return 1;
    }

    rpy->unpack(buf, buf);
    snprintf(rpyname, sizeof(rpyname), "%s_uddump.rpy", rpy_prefix);
    rpybuf_write(buf, rpyname);

    rpy->pack(buf, buf);
    snprintf(rpyname, sizeof(rpyname), "%s_uddupe.rpy", rpy_prefix);
    rpybuf_write(buf, rpyname);

    goto ret;
ret_fail:
    status = 1;
ret:
    rpybuf_destroy(buf);
    rpy_destroy(rpy);
    return 0;
}
