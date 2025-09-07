#pragma once

#include <map>
#include <string>
#define GLM_FORCE_CTOR_INIT
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

#include "../../logging/log_manager.h"
#include "../../utils/numeric_comparator.h"

namespace nl {

struct CTransparent {
    bool transparent{true};

    CTransparent() = default;
    explicit CTransparent(bool t) : transparent(t) {}

    void print() const { ENGINE_BACKTRACE("\t\ttransparent: {0}", transparent); }

    std::map<std::string, std::string, NumericComparator> to_map() const {
      std::map<std::string, std::string, NumericComparator> info;
      info["transparent"] = transparent ? "true" : "false";

      return info;
    }
};
}