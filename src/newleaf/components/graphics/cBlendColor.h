#pragma once

#include <map>
#include <string>
#define GLM_FORCE_CTOR_INIT
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

#include "../../logging/log_manager.h"
#include "../../utils/numeric_comparator.h"

namespace nl {

struct CBlendColor {
    glm::vec4 color{};
    float blend_factor{};

    CBlendColor() = default;
    explicit CBlendColor(glm::vec4&& c, float bf) : color(std::move(c)), blend_factor(bf) {}
    void print() const {
      ENGINE_BACKTRACE("\t\tcolor: {0}\n\t\t\t\t\t\tblend_factor: {1}", glm::to_string(color), blend_factor);
    }

    std::map<std::string, std::string, NumericComparator> to_map() const {
      std::map<std::string, std::string, NumericComparator> info;
      info["color"] = glm::to_string(color);
      info["blend_factor"] = std::to_string(blend_factor);
      return info;
    }
};
}