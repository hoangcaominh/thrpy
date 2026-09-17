#include "rpy/th06.h"
#include "crypt.h"
#include "kaitai/kaitaistream.h"
#include "ksy/th06.h"
#include "proto/th06.pb.h"
#include "proto/thdef.pb.h"
#include <google/protobuf/util/json_util.h>
#include <print>

static const size_t KEY_OFFSET = 14;
static const size_t CRYPT_OFFSET = 15;

static size_t unpack(RpyBuf* buf) {
    if (!buf || buf->size < CRYPT_OFFSET)
        return 0;

    rpy_decrypt06(
        buf->data + CRYPT_OFFSET,
        buf->size - CRYPT_OFFSET,
        buf->data[KEY_OFFSET]
    );
    return buf->size;
}

static size_t pack(RpyBuf* buf) {
    if (!buf || buf->size < CRYPT_OFFSET)
        return 0;

    rpy_encrypt06(
        buf->data + CRYPT_OFFSET,
        buf->size - CRYPT_OFFSET,
        buf->data[KEY_OFFSET]
    );
    return buf->size;
}

static void parse_replay_header(th06::Th06ReplayData& msg, const th06_t& kd) {
    msg.set_game("th06");
    msg.set_date(kd.date());
    msg.set_shot(static_cast<th06::Shot>(kd.shot()));
    msg.set_difficulty(static_cast<thdef::Difficulty>(kd.difficulty()));
    msg.set_score(kd.score());
    msg.set_name(kd.name());
    msg.set_slowdown(kd.slowdown());
}

static void parse_stage_header(th06::Th06ReplayData& msg, const th06_t& kd) {
    for (const auto sp : *kd.stage_offsets()) {
        const auto sh = sp->stage_header();
        if (sh == nullptr)
            continue;

        th06::Th06StageData* stage_msg = msg.add_stages();
        stage_msg->set_score(sh->score());
        stage_msg->set_seed(sh->seed());
        stage_msg->set_lives(sh->lives());
        stage_msg->set_bombs(sh->bombs());
        stage_msg->set_power(sh->power());
        stage_msg->set_rank(sh->rank());
    }
}

bool parse(const RpyBuf* buf, const RpyParseOptions* option) {
    std::string s(reinterpret_cast<char*>(buf->data), buf->size);
    kaitai::kstream ks(s);
    th06_t kd(&ks);
    th06::Th06ReplayData msg;

    if (option != NULL && option->include_replay_header)
        parse_replay_header(msg, kd);
    if (option != NULL && option->include_stage_header)
        parse_stage_header(msg, kd);

    std::string res;
    auto status = google::protobuf::util::MessageToJsonString(msg, &res);
    if (!status.ok()) {
        std::println("{}", status.message());
        return false;
    }

    std::println("{}", res);
    return true;
}

void rpy_th06(Rpy* rpy) {
    if (!rpy)
        return;
    rpy->gamecode = TH06;
    rpy->unpack_fn = unpack;
    rpy->pack_fn = pack;
    rpy->parse_fn = parse;
}
