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
static const size_t KEY_OFFSET_NC = 0x12;
static const size_t CRYPT_OFFSET_NC = 0x13;

static th06::Version get_version(const RpyBuf* buf) {
    if (buf->size < 6)
        return th06::VERSION_UNDEFINED;

    uint16_t version = *(uint16_t*)(buf->data + 4);
    if (version == 0x0102) // Original
        return th06::VERSION_ORIGINAL;
    else if (version == 0x0103) // Classic
        return th06::VERSION_CLASSIC;
    else if (version >= 0x010B) // New Classic
        return th06::VERSION_NEW_CLASSIC;

    // Rip replay
    return th06::VERSION_UNDEFINED;
}

static size_t unpack_o(RpyBuf* buf) {
    if (!buf || buf->size < CRYPT_OFFSET)
        return 0;

    rpy_decrypt06(
        buf->data + CRYPT_OFFSET,
        buf->size - CRYPT_OFFSET,
        buf->data[KEY_OFFSET]
    );
    return buf->size;
}

static size_t pack_o(RpyBuf* buf) {
    if (!buf || buf->size < CRYPT_OFFSET)
        return 0;

    rpy_encrypt06(
        buf->data + CRYPT_OFFSET,
        buf->size - CRYPT_OFFSET,
        buf->data[KEY_OFFSET]
    );
    return buf->size;
}

static size_t unpack_nc(RpyBuf* buf) {
    if (!buf || buf->size < CRYPT_OFFSET_NC)
        return 0;

    rpy_decrypt06(
        buf->data + CRYPT_OFFSET_NC,
        buf->size - CRYPT_OFFSET_NC,
        buf->data[KEY_OFFSET_NC]
    );

    return buf->size;
}

static size_t pack_nc(RpyBuf* buf) {
    if (!buf || buf->size < CRYPT_OFFSET_NC)
        return 0;

    rpy_encrypt06(
        buf->data + CRYPT_OFFSET_NC,
        buf->size - CRYPT_OFFSET_NC,
        buf->data[KEY_OFFSET_NC]
    );
    return buf->size;
}

static size_t unpack(RpyBuf* buf) {
    size_t (*unpack_fn)(RpyBuf*);
    switch (get_version(buf)) {
        case th06::VERSION_ORIGINAL:
        case th06::VERSION_CLASSIC:
            unpack_fn = unpack_o;
            break;
        case th06::VERSION_NEW_CLASSIC:
            unpack_fn = unpack_nc;
            break;
        default:
            return 0;
    }
    return unpack_fn(buf);
}

static size_t pack(RpyBuf* buf) {
    size_t (*pack_fn)(RpyBuf*);
    switch (get_version(buf)) {
        case th06::VERSION_ORIGINAL:
        case th06::VERSION_CLASSIC:
            pack_fn = pack_o;
            break;
        case th06::VERSION_NEW_CLASSIC:
            pack_fn = pack_nc;
            break;
        default:
            return 0;
    }
    return pack_fn(buf);
}

static void parse_replay_header_original(th06::ReplayData* msg, const void* data) {
    const th06_o_t* body = reinterpret_cast<const th06_o_t*>(data);
    msg->set_date(body->date());
    msg->set_shot(static_cast<th06::Shot>(body->shot()));
    msg->set_difficulty(static_cast<thdef::Difficulty>(body->difficulty()));
    msg->set_score(body->score());
    msg->set_name(body->name());
    msg->set_slowdown(body->slowdown());
}

static void parse_replay_header_classic(th06::ReplayData* msg, const void* data) {
    const th06_c_t* body = reinterpret_cast<const th06_c_t*>(data);
    msg->set_date(body->date());
    msg->set_shot(static_cast<th06::Shot>(body->shot()));
    msg->set_difficulty(static_cast<thdef::Difficulty>(body->difficulty()));
    msg->set_score(body->score());
    msg->set_name(body->name());
    msg->set_slowdown(body->slowdown());
}

static void parse_replay_header_new_classic(th06::ReplayData* msg, const void* data) {
    const th06_nc_t* body = reinterpret_cast<const th06_nc_t*>(data);
    msg->set_date(body->date());
    msg->set_shot(static_cast<th06::Shot>(body->shot()));
    msg->set_difficulty(static_cast<thdef::Difficulty>(body->difficulty()));
    msg->set_score(body->score());
    msg->set_name(body->name());
    msg->set_slowdown(body->slowdown());
}

static void parse_stage_header_original(th06::ReplayData* msg, const void* data) {
    const th06_o_t* body = reinterpret_cast<const th06_o_t*>(data);
    for (size_t i = 0; i < body->stage_offsets()->size(); i++) {
        const auto sh = (*body->stage_offsets())[i]->stage_header();
        if (sh == nullptr)
            continue;

        th06::StageData* stage_msg = msg->add_stages();
        stage_msg->set_stage(static_cast<thdef::Stage>(i));
        stage_msg->set_score(sh->score());
        stage_msg->set_seed(sh->seed());
        stage_msg->set_lives(sh->lives());
        stage_msg->set_bombs(sh->bombs());
        stage_msg->set_power(sh->power());
        stage_msg->set_rank(sh->rank());
    }
}

static void parse_stage_header_classic(th06::ReplayData* msg, const void* data) {
    const th06_c_t* body = reinterpret_cast<const th06_c_t*>(data);
    for (size_t i = 0; i < body->stage_offsets()->size(); i++) {
        const auto sh = (*body->stage_offsets())[i]->stage_header();
        if (sh == nullptr)
            continue;

        th06::StageData* stage_msg = msg->add_stages();
        stage_msg->set_stage(static_cast<thdef::Stage>(i));
        stage_msg->set_score(sh->score());
        stage_msg->set_seed(sh->seed());
        stage_msg->set_lives(sh->lives());
        stage_msg->set_bombs(sh->bombs());
        stage_msg->set_power(sh->power());
        stage_msg->set_rank(sh->rank());
    }
}

static void parse_stage_header_new_classic(th06::ReplayData* msg, const void* data) {
    const th06_nc_t* body = reinterpret_cast<const th06_nc_t*>(data);
    for (size_t i = 0; i < body->stage_offsets()->size(); i++) {
        const auto sh = (*body->stage_offsets())[i]->stage_header();
        if (sh == nullptr)
            continue;

        th06::StageData* stage_msg = msg->add_stages();
        stage_msg->set_stage(static_cast<thdef::Stage>(i));
        stage_msg->set_score(sh->score());
        stage_msg->set_seed(sh->seed());
        stage_msg->set_lives(sh->lives());
        stage_msg->set_bombs(sh->bombs());
        stage_msg->set_power(sh->power());
    }
}

static bool parse(const RpyBuf* buf, const RpyParseOptions* option) {
    std::string s(reinterpret_cast<char*>(buf->data), buf->size);
    kaitai::kstream ks(s);
    th06_t kd(&ks);
    th06::ReplayData msg;
    void (*parse_replay_header)(th06::ReplayData*, const void*);
    void (*parse_stage_header)(th06::ReplayData*, const void*);

    th06::Version version = get_version(buf);
    switch (version) {
        case th06::VERSION_ORIGINAL:
            parse_replay_header = parse_replay_header_original;
            parse_stage_header = parse_stage_header_original;
            break;
        case th06::VERSION_CLASSIC:
            parse_replay_header = parse_replay_header_classic;
            parse_stage_header = parse_stage_header_classic;
            break;
        case th06::VERSION_NEW_CLASSIC:
            parse_replay_header = parse_replay_header_new_classic;
            parse_stage_header = parse_stage_header_new_classic;
            break;
        default:
            return false;
    }

    if (option != NULL && option->include_replay_header) {
        msg.set_game(thdef::GameId::TH06);
        msg.set_version(version);
        parse_replay_header(&msg, kd.body());
    }
    if (option != NULL && option->include_stage_header)
        parse_stage_header(&msg, kd.body());

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
