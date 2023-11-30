
#pragma once

#include <glm/glm.hpp>

namespace potatoengine {
struct CMaterial {
    glm::vec3 ambient{glm::vec3(1.f)};
    glm::vec3 diffuse{glm::vec3(1.f)};
    glm::vec3 specular{glm::vec3(1.f)};
    float shininess{32.f};

    CMaterial() = default;
    explicit CMaterial(glm::vec3&& a, glm::vec3&& d, glm::vec3&& s, float sh)
        : ambient(std::move(a)), diffuse(std::move(d)), specular(std::move(s)), shininess(sh) {}

    void print() const {
        ENGINE_TRACE("\t\tambient: {0}\n\t\t\t\tdiffuse: {1}\n\t\t\t\tspecular: {2}\n\t\t\t\tshininess: {3}", ambient, diffuse, specular, shininess);
    }
};
}