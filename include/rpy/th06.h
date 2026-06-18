#pragma once

#include "rpy.h"
#include "ksy/th06.h"

class Rpy06 final : public RpyBase {
public:
    RpyBuf decompile(const RpyBuf &data) override;
    RpyBuf compile(const RpyBuf &data) override;
};

th06_t th06_deserialize(const RpyBuf& data);

// TODO: Implement
RpyBuf th06_serialize(const th06_t& data);
