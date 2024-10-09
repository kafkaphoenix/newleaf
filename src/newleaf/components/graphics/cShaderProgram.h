
#pragma once

#include <map>
#include <string>

#include "../../logging/log_manager.h"
#include "../../utils/numeric_comparator.h"

namespace nl {

struct CShaderProgram {
    std::string name;
    bool visible{true};

    CShaderProgram() = default;
    explicit CShaderProgram(std::string&& n, bool iv)
      : name(std::move(n)), visible(iv) {}

    void print() const {
      ENGINE_BACKTRACE("\t\tname: {0}\n\t\t\t\t\t\tvisible: {1}", name,
                       visible);
    }

    std::map<std::string, std::string, NumericComparator> to_map() const {
      std::map<std::string, std::string, NumericComparator> info;
      info["name"] = name;
      info["visible"] = visible ? "true" : "false";

      return info;
    }
};
}