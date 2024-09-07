#pragma once

#define GLM_FORCE_CTOR_INIT

#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <map>
#include <string>

#include "../../core/log_manager.h"
#include "../../utils/numeric_comparator.h"

namespace nl::components {

struct CLight {
    enum class Type { // https://docs.unity3d.com/Manual/Lighting.html
      Point,
      Spot,
      Directional,
      Area,
    };

    std::string _type;
    Type type;
    glm::vec3 color{glm::vec3(1.f)};
    float intensity{};
    float range{};
    float inner_cone_angle{};
    float outer_cone_angle{};

    CLight() = default;
    explicit CLight(std::string&& t) : _type(std::move(t)) {}
    explicit CLight(std::string&& t, glm::vec3&& c, float i, float r, float ica,
                    float oca)
      : _type(std::move(t)), color(std::move(c)), intensity(i), range(r),
        inner_cone_angle(ica), outer_cone_angle(oca) {}

    void print() const {
      ENGINE_BACKTRACE(
        "\t\ttype: {0}\n\t\t\t\t\t\tcolor: {1}\n\t\t\t\t\t\tintensity: "
        "{2}\n\t\t\t\t\t\trange: "
        "{3}\n\t\t\t\t\t\tinner_cone_angle: {4}\n\t\t\t\t\t\touter_cone_angle: "
        "{5}",
        _type, glm::to_string(color), intensity, range, inner_cone_angle,
        outer_cone_angle);
    }

    std::map<std::string, std::string, NumericComparator> to_map() const {
      std::map<std::string, std::string, NumericComparator> info;
      info["type"] = _type;
      info["color"] = glm::to_string(color);
      info["intensity"] = std::to_string(intensity);
      info["range"] = std::to_string(range);
      info["inner_cone_angle"] = std::to_string(inner_cone_angle);
      info["outer_cone_angle"] = std::to_string(outer_cone_angle);

      return info;
    }

    void set_light_type() {
      if (_type == "directional") {
        type = Type::Directional;
      } else if (_type == "point") {
        type = Type::Point;
      } else if (_type == "spot") {
        type = Type::Spot;
      } else if (_type == "area") {
        type = Type::Area;
      } else {
        ENGINE_ASSERT(false, "unknown light type {}", _type);
      }
    }
};
}

template <>
inline void nl::SceneManager::on_component_added(entt::entity e,
                                                 components::CLight& c) {
  c.set_light_type();

  m_registry.replace<components::CLight>(e, c);
}