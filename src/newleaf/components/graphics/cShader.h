#pragma once

#include <map>
#include <string>

#include "../../logging/log_manager.h"
#include "../../utils/numeric_comparator.h"

namespace nl {

struct CShader {
    std::string id;
    bool visible{true};

    CShader() = default;
    explicit CShader(std::string&& id, bool iv) : id(std::move(id)), visible(iv) {}

    void print() const { ENGINE_BACKTRACE("\t\tid: {0}\n\t\t\t\t\t\tvisible: {1}", id, visible); }

    std::map<std::string, std::string, NumericComparator> to_map() const {
      std::map<std::string, std::string, NumericComparator> info;
      info["id"] = id;
      info["visible"] = visible ? "true" : "false";

      return info;
    }
};
}
