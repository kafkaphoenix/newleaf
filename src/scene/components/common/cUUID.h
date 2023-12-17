#pragma once

#include "pch.h"

namespace potatoengine {

struct CUUID {
    uint32_t uuid{};

    void print() const { ENGINE_BACKTRACE("\t\tuuid: {}", uuid); }
};
}
