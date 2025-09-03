#pragma once

#include <map>
#include <string>

#define GLM_FORCE_CTOR_INIT
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

#include "../../logging/log_manager.h"
#include "../../utils/numeric_comparator.h"

namespace nl {

struct CFog {
    glm::vec3 color{};
    float density{};
    float gradient{};

    CFog() = default;
    explicit CFog(glm::vec3&& fc, float fd, float fg) : color(std::move(fc)), density(fd), gradient(fg) {}

    void print() const {
      ENGINE_BACKTRACE("\t\tcolor: {0}\n\t\t\t\t\t\tdensity: {1}\n\t\t\t\t\t\tgradient: {2}", glm::to_string(color),
                       density, gradient);
    }

    std::map<std::string, std::string, NumericComparator> to_map() const {
      std::map<std::string, std::string, NumericComparator> info;
      info["color"] = glm::to_string(color);
      info["density"] = std::to_string(density);
      info["gradient"] = std::to_string(gradient);

      return info;
    }
};
}