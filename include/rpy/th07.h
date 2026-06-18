#pragma once

#include "rpy.h"
#include "ksy/th07.h"

class Rpy07 final : public RpyBase {
public:
    RpyBuf decompile(const RpyBuf &data) override;
    RpyBuf compile(const RpyBuf &data) override;
};

th07_t th07_deserialize(const RpyBuf& data);

// TODO: Implement
RpyBuf th07_serialize(const th07_t& data);
