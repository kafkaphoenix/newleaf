#pragma once

#include <map>
#include <string>
#include <vector>

#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

#include "../../graphics/shape_factory.h"
#include "../../logging/log_manager.h"
#include "../../utils/assert.h"
#include "../../utils/map_json_serializer.h"
#include "../../utils/numeric_comparator.h"
#include "cMesh.h"
#include "cShape.h"

namespace nl {

struct CShape {
    enum class Type { triangle, rectangle, cube, circle };

    std::string _type;
    Type type;
    glm::vec3 size{glm::vec3(1.f)};
    std::vector<CMesh> meshes;
    bool repeat_texture{};

    CShape() = default;
    explicit CShape(Type t, glm::vec3&& d, std::vector<CMesh>&& m,
                    bool r = false)
      : type(t), size(d), meshes(std::move(m)), repeat_texture(r) {}

    void print() const {
      ENGINE_BACKTRACE(
        "\t\ttype: {0}\n\t\t\t\t\t\tsize: {1}\n\t\t\t\t\t\tmeshes: "
        "{2}\n\t\t\t\t\t\trepeat_texture: {3}",
        _type, glm::to_string(size), meshes.size(), repeat_texture);
    }

    std::map<std::string, std::string, NumericComparator> to_map() const {
      std::map<std::string, std::string, NumericComparator> info;
      info["type"] = _type;
      info["size"] = glm::to_string(size);
      for (uint32_t i = 0; i < meshes.size(); ++i) {
        info["mesh_" + std::to_string(i)] = get_mesh_info(i);
      }
      info["repeat_texture"] = repeat_texture ? "true" : "false";

      return info;
    }

    std::string get_mesh_info(uint32_t index) const {
      return map_to_json(meshes.at(index).to_map());
    }

    void create_mesh() {
      ENGINE_ASSERT(size.x > 0.f and (size.y > 0.f or _type == "triangle"),
                    "shape witdh and height must be greater than 0");
      ENGINE_ASSERT(size.z > 0.f or _type not_eq "cube",
                    "cube depth must be greater than 0");
      CMesh mesh;
      if (_type == "triangle") {
        type = CShape::Type::triangle;
        mesh.vao = ShapeFactory::create_triangle(size.x);
        mesh.vertex_type = "shape";
      } else if (_type == "rectangle") {
        type = CShape::Type::rectangle;
        mesh.vao =
          ShapeFactory::create_rectangle(size.x, size.y, repeat_texture);
        mesh.vertex_type = "shape";
      } else if (_type == "cube") {
        type = CShape::Type::cube;
        mesh.vao =
          ShapeFactory::create_cube(size.x, size.y, size.z, repeat_texture);
        mesh.vertex_type = "shape";
      } else if (_type == "circle") {
        type = CShape::Type::circle;
        mesh.vao = ShapeFactory::create_circle(size.x, size.y);
        mesh.vertex_type = "shape";
      } else {
        ENGINE_ASSERT(false, "unknown shape type {}", _type);
      }
      meshes.emplace_back(std::move(mesh));
    }
};
}

template <>
inline void nl::SceneManager::on_component_added(entt::entity e, CShape& c) {
  c.create_mesh();

  m_registry.replace<CShape>(e, c);
}