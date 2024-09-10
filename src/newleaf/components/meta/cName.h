#pragma once

#include "../../logging/log_manager.h"
#include "../../utils/numeric_comparator.h"

#include <map>
#include <string>

namespace nl::components {

struct CName {
    std::string name;

    CName() = default;
    explicit CName(std::string&& n) : name(std::move(n)) {}

    void print() const { ENGINE_BACKTRACE("\t\tname: {}", name); }

    std::map<std::string, std::string, NumericComparator> to_map() const {
      std::map<std::string, std::string, NumericComparator> info;
      info["name"] = name;

      return info;
    }
};
}