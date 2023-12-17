#pragma once

#include "pch.h"

namespace potatoengine {

struct CDistanceFromCamera {
    int distance{};

    CDistanceFromCamera() = default;
    explicit CDistanceFromCamera(int d) : distance(d) {}

    void print() const { ENGINE_BACKTRACE("\t\tdistance: {}", distance); }
};
}