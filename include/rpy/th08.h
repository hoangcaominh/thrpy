#pragma once

#include "rpy.h"
#include "ksy/th08.h"

class Rpy08 final : public RpyBase {
public:
    RpyBuf decompile(const RpyBuf &data) override;
    RpyBuf compile(const RpyBuf &data) override;
};

th08_t th08_deserialize(const RpyBuf& buf);

// TODO: Implement
RpyBuf th08_serialize(const th08_t& data);
