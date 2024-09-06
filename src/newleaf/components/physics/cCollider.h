#pragma once

#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <map>
#include <string>

#include "../../core/log_manager.h"
#include "../../render/shape_factory.h"
#include "../../utils/numeric_comparator.h"
#include "graphics/cMesh.h"

namespace nl::components {

struct CCollider {
    enum class Type { Box, Capsule, Mesh, Sphere, Rectangle };

    std::string _type;
    Type type;
    glm::vec3 size{};
    CMesh mesh;
    glm::vec4 color{1.0f, 0.0f, 0.0f, 1.f};

    CCollider() = default;
    explicit CCollider(Type t, glm::vec3&& s) : type(t), size(std::move(s)) {}

    void print() const {
      ENGINE_BACKTRACE("\t\ttype: {0}\n\t\t\t\t\t\tsize: {1}", _type,
                       glm::to_string(size));
    }

    std::map<std::string, std::string, NumericComparator> to_map() const {
      std::map<std::string, std::string, NumericComparator> info;
      info["type"] = _type;
      info["size"] = glm::to_string(size);

      return info;
    }

    void set_type() {
      if (_type == "box") {
        type = Type::Box;
      } else if (_type == "capsule") {
        type = Type::Capsule;
      } else if (_type == "mesh") {
        type = Type::Mesh;
      } else if (_type == "sphere") {
        type = Type::Sphere;
      } else if (_type == "rectangle") {
        type = Type::Rectangle;
        mesh.vao = ShapeFactory::create_rectangle(size.x, size.y, false);
      } else {
        ENGINE_ASSERT(false, "Unknown collider type {}", _type);
      }
    }
};
}

template <>
inline void nl::SceneManager::on_component_added(entt::entity e, components::CCollider& c) {
  c.set_type();

  m_registry.replace<components::CCollider>(e, c);
}