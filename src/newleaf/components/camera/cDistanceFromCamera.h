#pragma once

#include "../../logging/log_manager.h"
#include "../../utils/numeric_comparator.h"

#include <map>
#include <string>

namespace nl::components {

struct CDistanceFromCamera {
    int distance{};

    CDistanceFromCamera() = default;
    explicit CDistanceFromCamera(int d) : distance(d) {}

    void print() const { ENGINE_BACKTRACE("\t\tdistance: {}", distance); }

    std::map<std::string, std::string, NumericComparator> to_map() const {
      std::map<std::string, std::string, NumericComparator> info;
      info["distance"] = std::to_string(distance);

      return info;
    }
};
}