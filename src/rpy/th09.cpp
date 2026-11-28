#include "rpy/th09.h"
#include "crypt.h"
#include "lzss.h"
#include <cstring>
#include "kaitai/kaitaistream.h"
#include "ksy/th09.h"
#include "proto/th09.pb.h"
#include "proto/thdef.pb.h"
#include "proto/parse_result.pb.h"
#include "msgutils.h"
#include <google/protobuf/timestamp.pb.h>

using namespace thrpy;

static const size_t KEY_OFFSET = 21;
static const size_t CRYPT_OFFSET = 24;
static const size_t LZSS_OFFSET = 192;
static const size_t USERDATA_PTR_OFFSET = 12;

static size_t unpack(RpyBuf* buf) {
    if (!buf || buf->size < LZSS_OFFSET || buf->capacity < LZSS_OFFSET)
        return 0;

    uint32_t userdata_offset = *(uint32_t*)(buf->data + USERDATA_PTR_OFFSET);
    if (buf->size < userdata_offset)
        return 0;

    size_t userdata_size = buf->size - userdata_offset;
    uint32_t comp_size = userdata_offset - LZSS_OFFSET;
    uint8_t* ptr_lzss = buf->data + LZSS_OFFSET;

    // Backup userdata section
    uint8_t* userdata = (uint8_t*)calloc(userdata_size, sizeof(*userdata));
    if (!userdata)
        return 0;
    memcpy(userdata, buf->data + userdata_offset, userdata_size);

    rpy_decrypt06(
        buf->data + CRYPT_OFFSET,
        userdata_offset - CRYPT_OFFSET,
        buf->data[KEY_OFFSET]
    );
    size_t decomp_size = rpy_unlzss(
        ptr_lzss,
        comp_size,
        ptr_lzss,
        buf->capacity - LZSS_OFFSET
    );
    memcpy(
        ptr_lzss + decomp_size,
        userdata,
        userdata_size
    );
    free(userdata);

    size_t sizediff = decomp_size - comp_size;
    *(uint32_t*)(buf->data + USERDATA_PTR_OFFSET) += sizediff;
    buf->size += sizediff;

    return buf->size;
}

static size_t pack(RpyBuf* buf) {
    if (!buf || buf->size < LZSS_OFFSET || buf->capacity < LZSS_OFFSET)
        return 0;

    // Could also use USER magic
    uint32_t decomp_size = *(uint32_t*)(buf->data + 28);
    uint32_t userdata_offset = LZSS_OFFSET + decomp_size;
    uint8_t* ptr_lzss = buf->data + LZSS_OFFSET;

    size_t comp_size = rpy_lzss(
        ptr_lzss,
        decomp_size,
        ptr_lzss,
        buf->capacity - LZSS_OFFSET
    );
    rpy_encrypt06(
        buf->data + CRYPT_OFFSET,
        LZSS_OFFSET - CRYPT_OFFSET + comp_size,
        buf->data[KEY_OFFSET]
    );
    memmove(
        ptr_lzss + comp_size,
        buf->data + userdata_offset,
        buf->size - userdata_offset
    );

    size_t sizediff = comp_size - decomp_size;
    *(uint32_t*)(buf->data + USERDATA_PTR_OFFSET) += sizediff;
    buf->size += sizediff;

    return buf->size;
}

static void set_date(th09::ReplayData* msg, const std::string& date) {
    google::protobuf::Timestamp* t = new google::protobuf::Timestamp(
        date_to_timestamp(date, "%y/%m/%d")
    );
    msg->set_allocated_timestamp(t);
}

static void parse_replay_header(th09::ReplayData* msg, th09_t* body) {
    msg->set_difficulty(static_cast<thdef::Difficulty>(body->difficulty()));
    set_date(msg, body->date());
    msg->set_name(body->name());

    // Get shot from the first existing stage in stage offsets
    for (size_t i = 0; i < body->stage_offsets()->size(); i++) {
        const auto sh = (*body->stage_offsets())[i]->stage_header();
        if (sh == nullptr)
            continue;
        msg->set_shot(static_cast<th09::Shot>(sh->shot()));
        msg->set_score(sh->score() * 10);
        break;
    }
}

static void parse_stage_header(th09::ReplayData* msg, const th09_t* body) {
    for (size_t i = 0; i < 9; i++) {
        const auto sh = (*body->stage_offsets())[i]->stage_header();
        if (sh == nullptr)
            continue;

        const auto sh2 = (*body->stage_offsets())[i + 10]->stage_header();

        th09::StageData* stage_msg = msg->add_stages();
        stage_msg->set_stage(static_cast<th09::Stage>(i));
        stage_msg->set_score(sh->score() * 10);
        stage_msg->set_lives(sh->lives());
        stage_msg->set_p2_shot(static_cast<th09::Shot>(sh2->shot()));
        stage_msg->set_p2_score(sh2->score() * 10);
    }
}

static bool parse(const RpyBuf* buf, const RpyParseOptions* option) {
    std::string s(reinterpret_cast<char*>(buf->data), buf->size);
    kaitai::kstream ks(s);
    th09_t kd(&ks);
    th09::ReplayData msg;

    if (option != nullptr && option->include_replay_header) {
        msg.set_game(thdef::GameId::TH09);
        msg.set_version(kd.version());
        parse_replay_header(&msg, &kd);
    }
    if (option != nullptr && option->include_stage_header)
        parse_stage_header(&msg, &kd);

    ParseResult res;
    res.set_allocated_result09(&msg);
    bool success = msg_write(res, option != nullptr ? option->outfile : nullptr);
    msg = *res.release_result09();
    return success;
}

void rpy_th09(Rpy* rpy) {
    if (!rpy)
        return;
    rpy->gamecode = TH09;
    rpy->unpack_fn = unpack;
    rpy->pack_fn = pack;
    rpy->parse_fn = parse;
}
