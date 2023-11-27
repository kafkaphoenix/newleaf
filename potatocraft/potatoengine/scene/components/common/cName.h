#pragma once

#include "potatoengine/pch.h"

namespace potatoengine {

struct CName {
    std::string name{};

    CName() = default;
    explicit CName(std::string&& n) : name(std::move(n)) {}

    void print() const {
        CORE_TRACE("\t\tname: {}", name);
    }
};
}