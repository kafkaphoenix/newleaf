#pragma once

#include <map>
#include <string>

#include "../../logging/log_manager.h"
#include "../../utils/numeric_comparator.h"

namespace nl {

struct CName {
    std::string id;

    CName() = default;
    explicit CName(std::string&& id) : id(std::move(id)) {}

    void print() const { ENGINE_BACKTRACE("\t\tid: {}", id); }

    std::map<std::string, std::string, NumericComparator> to_map() const {
      std::map<std::string, std::string, NumericComparator> info;
      info["id"] = id;

      return info;
    }
};
}