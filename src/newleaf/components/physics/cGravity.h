#pragma once

#include <map>
#include <string>

#include "../../logging/log_manager.h"
#include "../../utils/numeric_comparator.h"

namespace nl {

struct CGravity {
    float acceleration{9.81f};

    CGravity() = default;
    explicit CGravity(float a) : acceleration(a) {}

    void print() const {
      ENGINE_BACKTRACE("\t\tacceleration: {0}", acceleration);
    }

    std::map<std::string, std::string, NumericComparator> to_map() const {
      std::map<std::string, std::string, NumericComparator> info;
      info["acceleration"] = std::to_string(acceleration);

      return info;
    }
};
}