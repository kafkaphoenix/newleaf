#pragma once

#include "../pch.h"
#include "../utils/numeric_comparator.h"

namespace nl {

struct CUUID {
    uint32_t uuid{};

    void print() const { ENGINE_BACKTRACE("\t\tuuid: {}", uuid); }

    std::map<std::string, std::string, NumericComparator> to_map() const {
      std::map<std::string, std::string, NumericComparator> info;
      info["uuid"] = std::to_string(uuid);

      return info;
    }
};
}
