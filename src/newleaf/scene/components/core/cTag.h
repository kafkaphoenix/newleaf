#pragma once

#include "../pch.h"

#include "../utils/numeric_comparator.h"

namespace nl {

struct CTag {
    std::string tag;

    CTag() = default;
    explicit CTag(std::string&& t) : tag(std::move(t)) {}

    void print() const { ENGINE_BACKTRACE("\t\ttag: {}", tag); }

    std::map<std::string, std::string, NumericComparator> to_map() const {
      std::map<std::string, std::string, NumericComparator> info;
      info["tag"] = tag;

      return info;
    }
};
}