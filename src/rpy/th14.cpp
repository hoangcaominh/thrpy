#include "rpy/th14.h"
#include "thmodern.h"
#include <cstring>
#include "kaitai/kaitaistream.h"
#include "ksy/th14.h"
#include "proto/th14.pb.h"
#include "proto/thdef.pb.h"
#include <google/protobuf/timestamp.pb.h>
#include "msgutils.h"

static RpyModernKey key1 = { 0x400, 0x5C, 0xE1 };
static RpyModernKey key2 = { 0x100, 0x7D, 0x3A };

static size_t unpack(RpyBuf* buf) {
    return unpack_thmodern(buf, &key1, &key2);
}

static size_t pack(RpyBuf* buf) {
    return pack_thmodern(buf, &key1, &key2);
}

static void parse_replay_header(th14::ReplayData* msg, th14_t* data) {
    msg->set_shot(static_cast<th14::Shot>(data->shot() * 2 + data->subshot()));
    msg->set_difficulty(static_cast<thdef::Difficulty>(data->difficulty()));
    msg->set_score(data->score() * 10);
    google::protobuf::Timestamp* t = new google::protobuf::Timestamp();
    t->set_seconds(data->timestamp());
    msg->set_allocated_timestamp(t);
    msg->set_name(data->name());
    msg->set_slowdown(data->slowdown());
}

static void parse_stage_header(th14::ReplayData* msg, const th14_t* data) {
    for (const auto s : *data->stages()) {
        th14::StageData* stage_msg = msg->add_stages();
        stage_msg->set_stage(static_cast<thdef::Stage>(s->stage_num() - 1));
        stage_msg->set_score(s->score() * 10);
        stage_msg->set_graze(s->graze());
        stage_msg->set_piv(s->piv() / 1000 * 10);
        stage_msg->set_power(s->power());
        stage_msg->set_lives(s->lives());
        stage_msg->set_life_pieces(s->life_pieces());
        stage_msg->set_bombs(s->bombs());
        stage_msg->set_bomb_pieces(s->bomb_pieces());
    }
}

static bool parse(const RpyBuf* buf, const RpyParseOptions* option) {
    std::string s(reinterpret_cast<char*>(buf->data), buf->size);
    kaitai::kstream ks(s);
    th14_t kd(&ks);
    th14::ReplayData msg;

    if (option != NULL && option->include_replay_header) {
        msg.set_game(thdef::GameId::TH14);
        msg.set_version(kd.version());
        parse_replay_header(&msg, &kd);
    }
    if (option != NULL && option->include_stage_header)
        parse_stage_header(&msg, &kd);

    return msg_write(msg, option != nullptr ? option->outfile : nullptr);
}

void rpy_th14(Rpy* rpy) {
    if (!rpy)
        return;
    rpy->gamecode = TH14;
    rpy->unpack_fn = unpack;
    rpy->pack_fn = pack;
    rpy->parse_fn = parse;
}
