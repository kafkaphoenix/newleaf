#pragma once

#define GLM_FORCE_CTOR_INIT

#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

#include "../utils/numeric_comparator.h"

namespace nl {

struct CSkybox {
    bool enable_fog{};
    glm::vec3 fog_color{};
    float fog_density{};
    float fog_gradient{};
    float rotation_speed{};

    CSkybox() = default;
    explicit CSkybox(bool uf, glm::vec3&& fc, float fd, float fg, float rs)
      : enable_fog(uf), fog_color(std::move(fc)), fog_density(fd),
        fog_gradient(fg), rotation_speed(rs) {}

    void print() const {
      ENGINE_BACKTRACE(
        "\t\tenable_fog: {0}\n\t\t\t\t\t\tfog_color: "
        "{1}\n\t\t\t\t\t\tfog_density: {2}\n\t\t\t\t\t\tfog_gradient: "
        "{3}\n\t\t\t\t\t\trotation_speed: {4}",
        enable_fog, glm::to_string(fog_color), fog_density, fog_gradient,
        rotation_speed);
    }

    std::map<std::string, std::string, NumericComparator> to_map() const {
      std::map<std::string, std::string, NumericComparator> info;
      info["enable_fog"] = enable_fog ? "true" : "false";
      info["fog_color"] = glm::to_string(fog_color);
      info["fog_density"] = std::to_string(fog_density);
      info["fog_gradient"] = std::to_string(fog_gradient);
      info["rotation_speed"] = std::to_string(rotation_speed);

      return info;
    }
};
}