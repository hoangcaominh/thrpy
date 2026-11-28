#include "rpy/th20.h"
#include "rpy/thmodern20.h"
#include <cstring>
#include "kaitai/kaitaistream.h"
#include "ksy/th20.h"
#include "proto/th20.pb.h"
#include "proto/thdef.pb.h"
#include "proto/parse_result.pb.h"
#include <google/protobuf/timestamp.pb.h>
#include "msgutils.h"

using namespace thrpy;

static RpyModernKey key1 = { 0x400, 0x5C, 0xE1 };
static RpyModernKey key2 = { 0x100, 0x7D, 0x3A };

static size_t unpack(RpyBuf* buf) {
    return unpack_thmodern20(buf, &key1, &key2);
}

static size_t pack(RpyBuf* buf) {
    return pack_thmodern20(buf, &key1, &key2);
}

static void parse_replay_header(th20::ReplayData* msg, th20_t* data) {
    const auto& stones = *data->stones();
    msg->set_shot(static_cast<th20::Shot>(data->shot() * 8 + stones[0]));
    msg->set_stone_main(static_cast<th20::Stone>(stones[0]));
    msg->set_stone_unfocused(static_cast<th20::Stone>(stones[1]));
    msg->set_stone_focused(static_cast<th20::Stone>(stones[2]));
    msg->set_stone_passive(static_cast<th20::Stone>(stones[3]));
    msg->set_difficulty(static_cast<thdef::Difficulty>(data->difficulty()));
    msg->set_score(data->score() * 10);
    google::protobuf::Timestamp* t = new google::protobuf::Timestamp();
    t->set_seconds(data->timestamp());
    msg->set_allocated_timestamp(t);
    msg->set_name(data->name());
    msg->set_slowdown(data->slowdown());
}

static void parse_stage_header(th20::ReplayData* msg, const th20_t* data) {
    // TODO: Complete this when th10 stage data has progress
}

static bool parse(const RpyBuf* buf, const RpyParseOptions* option) {
    std::string s(reinterpret_cast<char*>(buf->data), buf->size);
    kaitai::kstream ks(s);
    th20_t kd(&ks);
    th20::ReplayData msg;

    if (option != nullptr && option->include_replay_header) {
        msg.set_game(thdef::GameId::TH20);
        msg.set_version(kd.version());
        parse_replay_header(&msg, &kd);
    }
    if (option != nullptr && option->include_stage_header)
        parse_stage_header(&msg, &kd);

    ParseResult res;
    res.set_allocated_result20(&msg);
    bool success = msg_write(res, option != nullptr ? option->outfile : nullptr);
    msg = *res.release_result20();
    return success;
}

void rpy_th20(Rpy* rpy) {
    if (!rpy)
        return;
    rpy->gamecode = TH20;
    rpy->unpack_fn = unpack;
    rpy->pack_fn = pack;
    rpy->parse_fn = parse;
}
