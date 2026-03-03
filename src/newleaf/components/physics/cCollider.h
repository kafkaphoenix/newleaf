#pragma once

#include <map>
#include <string>
#include <utility>

#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

#include "../../graphics/shape_factory.h"
#include "../../logging/log_manager.h"
#include "../../scene/scene_manager.h"
#include "../../utils/assert.h"
#include "../../utils/numeric_comparator.h"
#include "../graphics/cMesh.h"

namespace nl {

struct CCollider {
    enum class Type { box, capsule, mesh, sphere, rectangle };

    std::string _type;
    Type type;
    glm::vec3 size{};
    CMesh mesh;
    glm::vec4 color{1.0f, 0.0f, 0.0f, 1.f};
    bool display_hitbox{};

    CCollider() = default;
    explicit CCollider(Type t, glm::vec3&& s, glm::vec4&& c, bool d)
      : type(t), size(std::move(s)), color(std::move(c)), display_hitbox(d) {}

    void print() const {
      ENGINE_BACKTRACE("\t\ttype: {0}\n\t\t\t\t\t\tsize: {1}\n\t\t\t\t\tcolor: {2}\n\t\t\t\t\tdisplay_hitbox: {3}",
                       _type, glm::to_string(size), glm::to_string(color), display_hitbox);
    }

    std::map<std::string, std::string, NumericComparator> to_map() const {
      std::map<std::string, std::string, NumericComparator> info;
      info["type"] = _type;
      info["size"] = glm::to_string(size);
      info["color"] = glm::to_string(color);
      info["display_hitbox"] = display_hitbox ? "true" : "false";

      return info;
    }

    // TODO only 2d colliders for now? check with cube, rethink logic and component and shader
    void set_type() {
      if (_type == "box") {
        type = Type::box;
      } else if (_type == "capsule") {
        type = Type::capsule;
      } else if (_type == "mesh") {
        type = Type::mesh;
      } else if (_type == "sphere") {
        type = Type::sphere;
      } else if (_type == "rectangle") {
        type = Type::rectangle;
        mesh = ShapeFactory::create_rectangle(size.x, size.y, false);
      } else {
        ENGINE_ASSERT(false, "unknown collider type {}", _type);
      }
    }
};
}

template <> inline void nl::SceneManager::on_component_added(entt::entity e, CCollider& c) {
  c.set_type();

  // mesh is move only (it owns VAO unique ptr) so components containing mesh cannot be copyable or assignable,
  // replace method uses assignment operator so we need to remove and emplace to update the mesh when type is set and
  // mesh is created
  m_registry.remove<CCollider>(e);
  m_registry.emplace<CCollider>(e, std::move(c));
}