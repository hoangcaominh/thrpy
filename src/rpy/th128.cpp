#include "rpy/th128.h"
#include "rpy.h"
#include "thmodern.h"
#include <cstring>
#include "kaitai/kaitaistream.h"
#include "ksy/th128.h"
#include "proto/th128.pb.h"
#include "proto/thdef.pb.h"
#include <google/protobuf/timestamp.pb.h>
#include <google/protobuf/util/json_util.h>
#include <print>

static RpyModernKey key1 = { 0x800, 0x5E, 0xE7 };
static RpyModernKey key2 = { 0x80, 0x7D, 0x36 };

static size_t unpack(RpyBuf* buf) {
    return unpack_thmodern(buf, &key1, &key2);
}

static size_t pack(RpyBuf* buf) {
    return pack_thmodern(buf, &key1, &key2);
}

static void parse_replay_header(th128::ReplayData* msg, th128_t* data) {
    msg->set_difficulty(static_cast<thdef::Difficulty>(data->difficulty()));
    msg->set_route(static_cast<th128::Route>(data->route()));
    msg->set_score(data->score() * 10);
    google::protobuf::Timestamp* t = new google::protobuf::Timestamp();
    t->set_seconds(data->timestamp());
    msg->set_allocated_timestamp(t);
    msg->set_name(data->name());
    msg->set_slowdown(data->slowdown());
}

static void parse_stage_header(th128::ReplayData* msg, const th128_t* data) {
    for (const auto s : *data->stages()) {
        th128::StageData* stage_msg = msg->add_stages();
        stage_msg->set_stage(static_cast<th128::Stage>(s->stage_num() - 1));
        stage_msg->set_score(s->score() * 10);
        stage_msg->set_graze(s->graze());
        stage_msg->set_motivation(s->motivation());
        stage_msg->set_perfect_freeze(s->perfect_freeze());
        stage_msg->set_frozen_area(s->frozen_area());
    }
}

static bool parse(const RpyBuf* buf, const RpyParseOptions* option) {
    std::string s(reinterpret_cast<char*>(buf->data), buf->size);
    kaitai::kstream ks(s);
    th128_t kd(&ks);
    th128::ReplayData msg;

    if (option != NULL && option->include_replay_header) {
        msg.set_game(thdef::GameId::TH128);
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

void rpy_th128(Rpy* rpy) {
    if (!rpy)
        return;
    rpy->gamecode = TH128;
    rpy->unpack_fn = unpack;
    rpy->pack_fn = pack;
    rpy->parse_fn = parse;
}
