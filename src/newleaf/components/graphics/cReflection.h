#pragma once

#include <map>
#include <string>

#include "../../logging/log_manager.h"
#include "../../utils/numeric_comparator.h"

namespace nl {

struct CReflection {
    float reflectivity{};
    float refractivity{};

    CReflection() = default;
    explicit CReflection(float r, float i) : reflectivity(r), refractivity(i) {}

    void print() const {
      ENGINE_BACKTRACE("\t\treflectivity: {0}\n\t\t\t\t\t\trefractivity: {1}", reflectivity, refractivity);
    }

    std::map<std::string, std::string, NumericComparator> to_map() const {
      std::map<std::string, std::string, NumericComparator> info;
      info["reflectivity"] = std::to_string(reflectivity);
      info["refractivity"] = std::to_string(refractivity);

      return info;
    }
};
}