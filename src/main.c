#include "config.h"
#include "thrpy.h"
#include <stdbool.h>
#include <stdio.h>
#include <argp.h>
#include <argz.h>

#define OPT_NAME_PACK "pack"
#define OPT_KEY_PACK 'z'

#define OPT_NAME_UNPACK "unpack"
#define OPT_KEY_UNPACK 'x'

#define OPT_NAME_OUTPUT "output"
#define OPT_KEY_OUTPUT 'o'

#define OPT_NAME_INFO "info"
#define OPT_KEY_INFO 'i'

#define OPT_NAME_RAW "raw"
#define OPT_KEY_RAW 'R'

const char* argp_program_version = PROJECT_VERSION;

const struct argp_option OPT_PACK = { OPT_NAME_PACK, OPT_KEY_PACK, "FILE", 0, "Pack a replay. Ignores the R option." };
const struct argp_option OPT_UNPACK = { OPT_NAME_UNPACK, OPT_KEY_UNPACK, "FILE", 0, "Unpack a replay. Does nothing when the R option is specified." };
const struct argp_option OPT_OUTPUT = { OPT_NAME_OUTPUT, OPT_KEY_OUTPUT, "FILE", 0, "Output to file. Used when specifying the i option." };
const struct argp_option OPT_INFO = { OPT_NAME_INFO, OPT_KEY_INFO, "INFO", 0, "Show information inside a replay. Prints to stdout by default. Available options are (r)eplay, (s)tage, and (i)nput." };
const struct argp_option OPT_RAW = { OPT_NAME_RAW, OPT_KEY_RAW, NULL, 0, "Mark the input file as a raw replay. Used when specifying the i option." };

struct thrpy_args {
    char* argz;
    size_t argz_len;
    int pack_mode;
    char* pack_outfile;
    bool parse;
    RpyParseOptions parse_opts;
    bool is_raw;
};

static int parse_opt(int key, char* arg, struct argp_state* state) {
    struct thrpy_args* a = state->input;

    switch (key) {
        case OPT_KEY_PACK:
            if (a->pack_mode)
                argp_failure(state, 1, 0, "cannot pack and unpack replay in the same command");
            a->pack_mode = key;
            a->pack_outfile = arg;
            break;
        case OPT_KEY_UNPACK:
            if (a->pack_mode)
                argp_failure(state, 1, 0, "cannot pack and unpack replay in the same command");
            a->pack_mode = key;
            a->pack_outfile = arg;
            break;
        case OPT_KEY_OUTPUT:
            a->parse_opts.outfile = arg;
            break;
        case OPT_KEY_INFO:
            a->parse = true;
            if (strcmp(arg, "r") == 0 || strcmp(arg, "replay") == 0)
                a->parse_opts.include_replay_header = true;
            else if (strcmp(arg, "s") == 0 || strcmp(arg, "stage") == 0)
                a->parse_opts.include_stage_header = true;
            else if (strcmp(arg, "i") == 0 || strcmp(arg, "input") == 0)
                a->parse_opts.include_input_frames = true;
            else
                argp_failure(state, 1, 0, "unrecognized info option \"%s\"", arg);
            break;
        case OPT_KEY_RAW:
            a->is_raw = true;
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

    if (!thargs->pack_mode && !thargs->parse) {
        thargs->parse = true;
        thargs->parse_opts.include_replay_header = true;
    }

    if (thargs->pack_mode == OPT_KEY_UNPACK && !thargs->is_raw) {
        rpy_unpack(rpy, buf, buf);
        rpybuf_write(buf, thargs->pack_outfile);
    }

    if (thargs->parse) {
        if (!thargs->is_raw && thargs->pack_mode != OPT_KEY_UNPACK)
            rpy_unpack(rpy, buf, buf);
        rpy_parse(rpy, buf, &thargs->parse_opts);
    }

    if (thargs->pack_mode == OPT_KEY_PACK) {
        rpy_pack(rpy, buf, buf);
        rpybuf_write(buf, thargs->pack_outfile);
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
        OPT_INFO,
        OPT_RAW,
        { NULL },
    };

    struct argp argp = {
        opts,
        parse_opt,
        "FILE",
        "Process Touhou Project replay file."
    };
    struct thrpy_args thargs = {
        .pack_mode = 0,
        .parse = false,
        .parse_opts.outfile = NULL,
        .is_raw = false,
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
