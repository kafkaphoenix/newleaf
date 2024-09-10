#pragma once

#include "../../logging/log_manager.h"
#include "../../utils/numeric_comparator.h"

#include <map>
#include <string>

namespace nl::components {

struct CActiveCamera {
    bool dummy;

    void print() const { ENGINE_BACKTRACE("\t\tno data"); }

    std::map<std::string, std::string, NumericComparator> to_map() const {
      std::map<std::string, std::string, NumericComparator> info;
      return info;
    }
};
}