#pragma once

#include <map>
#include <string>

#include <entt/entt.hpp>
#define GLM_FORCE_CTOR_INIT
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

#include "../../logging/log_manager.h"
#include "../../scene/scene_manager.h"
#include "../../utils/assert.h"
#include "../../utils/numeric_comparator.h"

namespace nl {

struct CLight {
    enum class Type {
      area,
      cone,
      directional,
    };

    bool enabled{};
    std::string _type;
    Type type;
    glm::vec3 color{glm::vec3(1.f)};
    float intensity{};
    float range{};
    float inner_cone_angle{};
    float outer_cone_angle{};

    CLight() = default;
    explicit CLight(bool enabled, std::string&& t, glm::vec3&& c, float i, float r, float ica, float oca)
      : enabled(enabled), _type(std::move(t)), color(std::move(c)), intensity(i), range(r), inner_cone_angle(ica),
        outer_cone_angle(oca) {}

    void print() const {
      ENGINE_BACKTRACE("\t\tenabled: {0}\n\t\t\t\t\t\ttype: {1}\n\t\t\t\t\t\tcolor: {2}\n\t\t\t\t\t\tintensity: "
                       "{3}\n\t\t\t\t\t\trange: "
                       "{4}\n\t\t\t\t\t\tinner_cone_angle: {5}\n\t\t\t\t\t\touter_cone_angle: {6}",
                       enabled, _type, glm::to_string(color), intensity, range, inner_cone_angle, outer_cone_angle);
    }

    std::map<std::string, std::string, NumericComparator> to_map() const {
      std::map<std::string, std::string, NumericComparator> info;
      info["enabled"] = enabled ? "true" : "false";
      info["type"] = _type;
      info["color"] = glm::to_string(color);
      info["intensity"] = std::to_string(intensity);
      info["range"] = std::to_string(range);
      info["inner_cone_angle"] = std::to_string(inner_cone_angle);
      info["outer_cone_angle"] = std::to_string(outer_cone_angle);

      return info;
    }

    void set_light_type() {
      if (_type == "area") {
        type = Type::area;
      } else if (_type == "cone") {
        type = Type::cone;
      } else if (_type == "directional") {
        type = Type::directional;
      } else {
        ENGINE_ASSERT(false, "unknown light type {}", _type);
      }
    }
};
}

template <> inline void nl::SceneManager::on_component_added(entt::entity e, CLight& c) {
  c.set_light_type();

  m_registry.replace<CLight>(e, c);
}