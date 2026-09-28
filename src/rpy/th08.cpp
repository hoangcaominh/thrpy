#include "rpy/th08.h"
#include "crypt.h"
#include "lzss.h"
#include <cstring>
#include "kaitai/kaitaistream.h"
#include "ksy/th08.h"
#include "proto/th08.pb.h"
#include "proto/thdef.pb.h"
#include "msgutils.h"

static const size_t KEY_OFFSET = 21;
static const size_t CRYPT_OFFSET = 24;
static const size_t LZSS_OFFSET = 104;

static size_t unpack(RpyBuf* buf) {
    if (!buf || buf->size < LZSS_OFFSET || buf->capacity < LZSS_OFFSET)
        return 0;

    uint32_t userdata_offset = *(uint32_t*)(buf->data + 12);
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
    *(uint32_t*)(buf->data + 0xC) += sizediff;
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
    *(uint32_t*)(buf->data + 0xC) += sizediff;
    buf->size += sizediff;

    return buf->size;
}

static void parse_replay_header(th08::ReplayData* msg, th08_t* body) {
    msg->set_shot(static_cast<th08::Shot>(body->shot()));
    msg->set_difficulty(static_cast<thdef::Difficulty>(body->difficulty()));
    msg->set_score(body->score() * 10);
    msg->set_date(body->userdata()->date()->value());
    msg->set_name(body->name());
    msg->set_slowdown(body->slowdown());

    const auto &stage_offsets = *body->stage_offsets();
    if (stage_offsets[6]->stage_header() != nullptr)
        msg->set_final(th08::FINAL_A);
    else if (stage_offsets[7]->stage_header() != nullptr)
        msg->set_final(th08::FINAL_B);
}

static void parse_stage_header(th08::ReplayData* msg, const th08_t* body) {
    for (size_t i = 0; i < body->stage_offsets()->size(); i++) {
        const auto sh = (*body->stage_offsets())[i]->stage_header();
        if (sh == nullptr)
            continue;

        th08::StageData* stage_msg = msg->add_stages();
        stage_msg->set_stage(static_cast<th08::Stage>(i));
        stage_msg->set_score(sh->score() * 10);
        stage_msg->set_point_items(sh->point_items());
        stage_msg->set_graze(sh->graze());
        stage_msg->set_time(sh->time());
        stage_msg->set_piv(sh->piv());
        stage_msg->set_power(sh->power());
        stage_msg->set_lives(sh->lives());
        stage_msg->set_bombs(sh->bombs());
        // stage_msg->set_rank(sh->rank());
    }
}

static bool parse(const RpyBuf* buf, const RpyParseOptions* option) {
    std::string s(reinterpret_cast<char*>(buf->data), buf->size);
    kaitai::kstream ks(s);
    th08_t kd(&ks);
    th08::ReplayData msg;

    if (option != NULL && option->include_replay_header) {
        msg.set_game(thdef::GameId::TH08);
        msg.set_version(kd.version());
        parse_replay_header(&msg, &kd);
    }
    if (option != NULL && option->include_stage_header)
        parse_stage_header(&msg, &kd);

    return msg_write(msg, option != nullptr ? option->outfile : nullptr);
}

void rpy_th08(Rpy* rpy) {
    if (!rpy)
        return;
    rpy->gamecode = TH08;
    rpy->unpack_fn = unpack;
    rpy->pack_fn = pack;
    rpy->parse_fn = parse;
}
