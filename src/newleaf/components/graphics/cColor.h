#pragma once

#include <map>
#include <string>
#define GLM_FORCE_CTOR_INIT
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

#include "../../logging/log_manager.h"
#include "../../utils/numeric_comparator.h"

namespace nl {

struct CColor {
    glm::vec4 color{};

    CColor() = default;
    explicit CColor(float r, float g, float b, float a) : color(r, g, b, a) {}

    void print() const { ENGINE_BACKTRACE("\t\tcolor: {0}", glm::to_string(color)); }

    std::map<std::string, std::string, NumericComparator> to_map() const {
      std::map<std::string, std::string, NumericComparator> info;
      info["color"] = glm::to_string(color);

      return info;
    }
};
}