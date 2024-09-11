#pragma once

#include "../../logging/log_manager.h"
#include "../../utils/numeric_comparator.h"

#include <map>
#include <string>

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
