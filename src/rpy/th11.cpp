#include "rpy/th11.h"
#include "thmodern.h"
#include <cstring>
#include "kaitai/kaitaistream.h"
#include "ksy/th11.h"
#include "proto/th11.pb.h"
#include "proto/thdef.pb.h"
#include <google/protobuf/timestamp.pb.h>
#include <google/protobuf/util/json_util.h>
#include <print>

static RpyModernKey key1 = { 0x800, 0xAA, 0xE1 };
static RpyModernKey key2 = { 0x40, 0x3D, 0x7A };

static size_t unpack(RpyBuf* buf) {
    return unpack_thmodern(buf, &key1, &key2);
}

static size_t pack(RpyBuf* buf) {
    return pack_thmodern(buf, &key1, &key2);
}

static void parse_replay_header(th11::ReplayData* msg, th11_t* data) {
    msg->set_shot(static_cast<th11::Shot>(data->shot() * 3 + data->subshot()));
    msg->set_difficulty(static_cast<thdef::Difficulty>(data->difficulty()));
    msg->set_score(data->score() * 10);
    google::protobuf::Timestamp* t = new google::protobuf::Timestamp();
    t->set_seconds(data->timestamp());
    msg->set_allocated_timestamp(t);
    msg->set_name(data->name());
    msg->set_slowdown(data->slowdown());
}

static void parse_stage_header(th11::ReplayData* msg, const th11_t* data) {
    for (const auto s : *data->stages()) {
        th11::StageData* stage_msg = msg->add_stages();
        stage_msg->set_stage(static_cast<thdef::Stage>(s->stage_num() - 1));
        stage_msg->set_score(s->score() * 10);
        stage_msg->set_piv(s->piv());
        stage_msg->set_power(s->power());
        stage_msg->set_lives(s->lives());
        stage_msg->set_life_pieces(s->life_pieces());
    }
}

static bool parse(const RpyBuf* buf, const RpyParseOptions* option) {
    std::string s(reinterpret_cast<char*>(buf->data), buf->size);
    kaitai::kstream ks(s);
    th11_t kd(&ks);
    th11::ReplayData msg;

    if (option != NULL && option->include_replay_header) {
        msg.set_game(thdef::GameId::TH11);
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

void rpy_th11(Rpy* rpy) {
    if (!rpy)
        return;
    rpy->gamecode = TH11;
    rpy->unpack_fn = unpack;
    rpy->pack_fn = pack;
    rpy->parse_fn = parse;
}
