#pragma once

#include <map>
#include <string>

#include "../../logging/log_manager.h"
#include "../../utils/numeric_comparator.h"

namespace nl {

struct CSkybox {
    float rotation_speed{};

    CSkybox() = default;
    explicit CSkybox(float rs) : rotation_speed(rs) {}

    void print() const { ENGINE_BACKTRACE("\t\trotation_speed: {0}", rotation_speed); }

    std::map<std::string, std::string, NumericComparator> to_map() const {
      std::map<std::string, std::string, NumericComparator> info;
      info["rotation_speed"] = std::to_string(rotation_speed);

      return info;
    }
};
}