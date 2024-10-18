#pragma once

#include <map>
#include <string>

#define GLM_FORCE_CTOR_INIT
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

#include "../../logging/log_manager.h"
#include "../../utils/numeric_comparator.h"

namespace nl {

struct CTransform {
    glm::vec3 position{};
    glm::quat rotation{glm::identity<glm::quat>()};
    glm::vec3 scale{glm::vec3{1.f}};

    CTransform() = default;
    explicit CTransform(glm::vec3&& p, glm::quat&& r, glm::vec3&& s)
      : position(std::move(p)), rotation(std::move(r)), scale(std::move(s)) {}

    glm::mat4 calculate() const {
      // T * R * S
      return glm::scale(glm::translate(glm::mat4(1.f), position) *
                          glm::mat4_cast(rotation),
                        scale);
    }

    void rotate(float angle, const glm::vec3& axis) {
      rotation = glm::angleAxis(glm::radians(angle), axis) * rotation;
    }

    void rotate(const glm::quat& q) { rotation = q * rotation; }

    void print() const {
      ENGINE_BACKTRACE(
        "\t\tposition: {0}\n\t\t\t\t\t\trotation: {1}\n\t\t\t\t\t\tscale: {2}",
        glm::to_string(position), glm::to_string(rotation),
        glm::to_string(scale));
    }

    std::map<std::string, std::string, NumericComparator> to_map() const {
      std::map<std::string, std::string, NumericComparator> info;
      info["position"] = glm::to_string(position);
      info["rotation"] = glm::to_string(rotation);
      info["scale"] = glm::to_string(scale);

      return info;
    }
};
}