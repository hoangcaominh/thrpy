#include "rpy/th07.h"
#include "crypt.h"
#include "lzss.h"
#include "kaitai/kaitaistream.h"
#include "ksy/th07.h"
#include "proto/th07.pb.h"
#include "proto/thdef.pb.h"
#include <google/protobuf/util/json_util.h>
#include <print>

static const size_t KEY_OFFSET = 13;
static const size_t CRYPT_OFFSET = 16;
static const size_t LZSS_OFFSET = 84;

static size_t unpack(RpyBuf* buf) {
    if (!buf || buf->size < LZSS_OFFSET || buf->capacity < LZSS_OFFSET)
        return 0;

    rpy_decrypt06(
        buf->data + CRYPT_OFFSET,
        buf->size - CRYPT_OFFSET,
        buf->data[KEY_OFFSET]
    );
    size_t decomp_size = rpy_unlzss(
        buf->data + LZSS_OFFSET,
        buf->size - LZSS_OFFSET,
        buf->data + LZSS_OFFSET,
        buf->capacity - LZSS_OFFSET
    );

    buf->size = LZSS_OFFSET + decomp_size;
    return buf->size;
}

static size_t pack(RpyBuf* buf) {
    if (!buf || buf->size < LZSS_OFFSET || buf->capacity < LZSS_OFFSET)
        return 0;

    size_t comp_size = rpy_lzss(
        buf->data + LZSS_OFFSET,
        buf->size - LZSS_OFFSET,
        buf->data + LZSS_OFFSET,
        buf->capacity - LZSS_OFFSET
    );
    rpy_encrypt06(
        buf->data + CRYPT_OFFSET,
        LZSS_OFFSET - CRYPT_OFFSET + comp_size,
        buf->data[KEY_OFFSET]
    );

    buf->size = LZSS_OFFSET + comp_size;
    return buf->size;
}

static void parse_replay_header(th07::Th07ReplayData* msg, const th07_t* body) {
    msg->set_shot(static_cast<th07::Shot>(body->shot()));
    msg->set_difficulty(static_cast<thdef::Difficulty>(body->difficulty()));
    msg->set_score(body->score() * 10);
    msg->set_date(body->date());
    msg->set_name(body->name());
    msg->set_slowdown(body->slowdown());
}

static void parse_stage_header(th07::Th07ReplayData* msg, const th07_t* body) {
    for (const auto sp : *body->stage_offsets()) {
        const auto sh = sp->stage_header();
        if (sh == nullptr)
            continue;

        th07::Th07StageData* stage_msg = msg->add_stages();
        stage_msg->set_score(sh->score() * 10);
        stage_msg->set_point_items(sh->point_items());
        stage_msg->set_graze(sh->graze());
        stage_msg->set_piv(sh->piv());
        stage_msg->set_power(sh->power());
        stage_msg->set_lives(sh->lives());
        stage_msg->set_bombs(sh->bombs());
        stage_msg->set_cherry(sh->cherry());
        stage_msg->set_cherry_max(sh->cherry_max());
        // stage_msg->set_rank(sh->rank());
    }
}

static bool parse(const RpyBuf* buf, const RpyParseOptions* option) {
    std::string s(reinterpret_cast<char*>(buf->data), buf->size);
    kaitai::kstream ks(s);
    th07_t kd(&ks);
    th07::Th07ReplayData msg;

    if (option != NULL && option->include_replay_header) {
        msg.set_game(thdef::GameId::TH07);
        msg.set_version(kd.version());
        parse_replay_header(&msg, &kd);
    }
    if (option != NULL && option->include_stage_header)
        parse_stage_header(&msg, &kd);

    std::string res;
    auto status = google::protobuf::util::MessageToJsonString(msg, &res);
    if (!status.ok()) {
        std::println("{}", status.message());
        return false;
    }

    std::println("{}", res);
    return true;
}

void rpy_th07(Rpy* rpy) {
    if (!rpy)
        return;
    rpy->gamecode = TH07;
    rpy->unpack_fn = unpack;
    rpy->pack_fn = pack;
    rpy->parse_fn = parse;
}
