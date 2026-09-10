#include "config.h"
#include "thrpy.h"
#include <stdbool.h>
#include <stdio.h>
#include <argp.h>
#include <argz.h>

#define OPT_PACK_NAME "pack"
#define OPT_PACK_KEY 'z'

#define OPT_UNPACK_NAME "unpack"
#define OPT_UNPACK_KEY 'x'

#define OPT_OUTPUT_NAME "output"
#define OPT_OUTPUT_KEY 'o'

const char* argp_program_version = PROJECT_VERSION;

const struct argp_option OPT_PACK = { OPT_PACK_NAME, OPT_PACK_KEY, NULL, 0, "Pack a replay." };
const struct argp_option OPT_UNPACK = { OPT_UNPACK_NAME, OPT_UNPACK_KEY, NULL, 0, "Unpack a replay." };
const struct argp_option OPT_OUTPUT = { OPT_OUTPUT_NAME, OPT_OUTPUT_KEY, "FILE", 0, "Output to file. Default to " DEFAULT_OUTPUT_NAME " in the current directory." };

struct thrpy_args {
    char* outfile;
    char* argz;
    size_t argz_len;
    int mode;
};

static int parse_opt(int key, char* arg, struct argp_state* state) {
    struct thrpy_args* a = state->input;

    switch (key) {
        case OPT_PACK_KEY:
            if (a->mode)
                argp_failure(state, 1, 0, "cannot pack and unpack replay in the same command");
            a->mode = key;
            break;
        case OPT_UNPACK_KEY:
            if (a->mode)
                argp_failure(state, 1, 0, "cannot pack and unpack replay in the same command");
            a->mode = key;
            break;
        case OPT_OUTPUT_KEY:
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

    rpy_autoconf(rpy, buf);
    if (rpy->gamecode == THNA) {
        fprintf(stderr, "Unable to detect supported game.\n");
        return 1;
    }

    switch (thargs->mode) {
        case 0:
        case OPT_UNPACK_KEY:
            rpy_unpack(rpy, buf, buf);
            rpybuf_write(buf, thargs->outfile);
            break;
        case OPT_PACK_KEY:
            rpy_pack(rpy, buf, buf);
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
        OPT_PACK,
        OPT_UNPACK,
        OPT_OUTPUT,
        { NULL },
    };

    struct argp argp = {
        opts,
        parse_opt,
        "FILE",
        "Process Touhou Project replay file.\v"
        "By default the program assumes the unpack option, therefore it is not required to specify -x or --unpack in the command."
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
