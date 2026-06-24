#include "thrpy.h"
#include <stdbool.h>
#include <stdio.h>
#include <argp.h>
#include <argz.h>

#define DEFAULT_OUTPUT_NAME "output.bin"

#define OPT_PACK 'c'
#define OPT_UNPACK 'd'
#define OPT_OUTPUT 'o'

struct thrpy_args {
    char* outfile;
    char* argz;
    size_t argz_len;
    int mode;
};

static int parse_opt(int key, char* arg, struct argp_state* state) {
    struct thrpy_args* a = state->input;

    switch (key) {
        case OPT_PACK:
            if (a->mode)
                argp_failure(state, 1, 0, "cannot pack and unpack replay in the same command");
            a->mode = OPT_PACK;
            break;
        case OPT_UNPACK:
            if (a->mode)
                argp_failure(state, 1, 0, "cannot pack and unpack replay in the same command");
            a->mode = OPT_UNPACK;
            break;
        case 'o':
            a->outfile = arg;
            break;
        case ARGP_KEY_INIT:
            a->argz = NULL;
            a->argz_len = 0;
            break;
        case ARGP_KEY_ARG: {
            argz_add(&a->argz, &a->argz_len, arg);
            break;
        }
        case ARGP_KEY_END: {
            size_t count = argz_count(a->argz, a->argz_len);
            if (count != 1) {
                argp_failure(state, 1, 0, "invalid number of arguments");
            }
            break;
        }
    }

    return 0;
}

int do_command(char* file, struct thrpy_args* thargs) {
    Rpy* rpy;
    RpyBuf* buf;
    int status = 0;

    rpy = rpy_init();
    if (!rpy)
        goto ret_fail;

    buf = rpybuf_init();
    if (!buf)
        goto ret_fail;

    if (rpybuf_read(buf, file) == 0)
        goto ret_fail;

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
        default:
            fprintf(stderr, "Unable to detect supported game.\n");
            return 1;
    }

    switch (thargs->mode) {
        case 0:
        case OPT_UNPACK:
            rpy->unpack(buf, buf);
            rpybuf_write(buf, thargs->outfile);
            break;
        case OPT_PACK:
            rpy->pack(buf, buf);
            rpybuf_write(buf, thargs->outfile);
            break;
    }

    goto ret;
ret_fail:
    status = 1;
ret:
    rpybuf_destroy(buf);
    rpy_destroy(rpy);
    return 0;
}

int main(int argc, char* argv[]) {
    struct argp_option opts[] = {
        { "pack", OPT_PACK, NULL, 0, "Pack a replay." },
        { "unpack", OPT_UNPACK, NULL, 0, "Unpack a replay." },
        { "output", OPT_OUTPUT, "FILE", 0, "Output to file. Default to output.bin in the current directory." },
        { NULL },
    };
    struct argp argp = {
        opts,
        parse_opt,
        "FILE",
        "Process Touhou Project replay file\v"
        "By default the program assumes the unpack option, therefore it is not required to specify the -d or --unpack option."
    };
    struct thrpy_args thargs = {
        .outfile = DEFAULT_OUTPUT_NAME,
        .mode = 0,
    };

    if (argp_parse(&argp, argc, argv, 0, NULL, &thargs) == 0) {
        const char* prev = NULL;
        char* arg;
        while ((arg = argz_next(thargs.argz, thargs.argz_len, prev)) != NULL) {
            do_command(arg, &thargs);
            prev = arg;
        }
        free(thargs.argz);
    }

    return 0;
}
