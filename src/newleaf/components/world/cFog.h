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
    bool enabled{};
    glm::vec3 color{};
    float lower_limit{};
    float upper_limit{};

    CFog() = default;
    explicit CFog(bool enabled, glm::vec3&& fc, float fl, float fu)
      : enabled(enabled), color(std::move(fc)), lower_limit(fl), upper_limit(fu) {}

    void print() const {
      ENGINE_BACKTRACE(
        "\t\tenabled: {0}\n\t\t\t\t\t\tcolor: {1}\n\t\t\t\t\t\tlower_limit: {2}\n\t\t\t\t\t\tupper_limit: {3}", enabled,
        glm::to_string(color), lower_limit, upper_limit);
    }

    std::map<std::string, std::string, NumericComparator> to_map() const {
      std::map<std::string, std::string, NumericComparator> info;
      info["enabled"] = enabled ? "true" : "false";
      info["color"] = glm::to_string(color);
      info["lower_limit"] = std::to_string(lower_limit);
      info["upper_limit"] = std::to_string(upper_limit);

      return info;
    }
};
}