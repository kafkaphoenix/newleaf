#pragma once

#include <map>
#include <string>

#include "../../logging/log_manager.h"
#include "../../utils/numeric_comparator.h"

namespace nl {

struct CReflection {
    bool enabled{};
    float reflectivity{};
    float refractivity{};

    CReflection() = default;
    explicit CReflection(float r, float i, bool e) : reflectivity(r), refractivity(i), enabled(e) {}

    void print() const {
      ENGINE_BACKTRACE("\t\treflectivity: {0}\n\t\t\t\t\t\trefractivity: {1}\n\t\t\t\t\t\tenabled: {2}", reflectivity, refractivity, enabled);
    }

    std::map<std::string, std::string, NumericComparator> to_map() const {
      std::map<std::string, std::string, NumericComparator> info;
      info["reflectivity"] = std::to_string(reflectivity);
      info["refractivity"] = std::to_string(refractivity);
      info["enabled"] = enabled ? "true" : "false";

      return info;
    }
};
}