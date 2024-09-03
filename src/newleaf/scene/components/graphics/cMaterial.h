
#pragma once

#include "../utils/numeric_comparator.h"
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

namespace nl {
struct CMaterial {
    glm::vec3 ambient{glm::vec3(1.f)};
    glm::vec3 diffuse{glm::vec3(1.f)};
    glm::vec3 specular{glm::vec3(1.f)};
    float shininess{32.f};

    CMaterial() = default;
    explicit CMaterial(glm::vec3&& a, glm::vec3&& d, glm::vec3&& s, float sh)
      : ambient(std::move(a)), diffuse(std::move(d)), specular(std::move(s)),
        shininess(sh) {}

    void print() const {
      ENGINE_BACKTRACE(
        "\t\tambient: {0}\n\t\t\t\t\t\tdiffuse: "
        "{1}\n\t\t\t\t\t\tspecular: {2}\n\t\t\t\t\t\tshininess: {3}",
        glm::to_string(ambient), glm::to_string(diffuse),
        glm::to_string(specular), shininess);
    }

    std::map<std::string, std::string, NumericComparator> to_map() const {
      std::map<std::string, std::string, NumericComparator> info;
      info["ambient"] = glm::to_string(ambient);
      info["diffuse"] = glm::to_string(diffuse);
      info["specular"] = glm::to_string(specular);
      info["shininess"] = std::to_string(shininess);

      return info;
    }
};
}