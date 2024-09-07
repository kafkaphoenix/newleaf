#pragma once

#define GLM_FORCE_CTOR_INIT

#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <map>
#include <string>

#include "../../core/log_manager.h"
#include "../../utils/numeric_comparator.h"

namespace nl::components {

struct CInput {
    enum class Mode { _None, _3D, _2D };

    std::string _mode;
    Mode mode;
    float mouse_sensitivity = 0.1f;
    float translation_speed = 20.f;
    float vertical_speed = 45.f;
    float rotation_speed = 180.f;

    CInput() = default;
    explicit CInput(std::string&& m, float ms, float ts, float vs, float rs)
      : _mode(std::move(m)), mouse_sensitivity(ms), translation_speed(ts),
        vertical_speed(vs), rotation_speed(rs) {}

    void print() const {
      ENGINE_BACKTRACE(
        "\t\tmode: {0}\n\t\t\t\t\t\tmouse_sensitivity: "
        "{1}\n\t\t\t\t\t\ttranslation_speed: {2}\n\t\t\t\t\t\tvertical_speed: "
        "{3}\n\t\t\t\t\t\trotation_speed: {4}",
        _mode, mouse_sensitivity, translation_speed, vertical_speed,
        rotation_speed);
    }

    std::map<std::string, std::string, NumericComparator> to_map() const {
      std::map<std::string, std::string, NumericComparator> info;
      info["mode"] = _mode;
      info["mouse_sensitivity"] = std::to_string(mouse_sensitivity);
      info["translation_speed"] = std::to_string(translation_speed);
      info["vertical_speed"] = std::to_string(vertical_speed);
      info["rotation_speed"] = std::to_string(rotation_speed);

      return info;
    }

    void set_mode() {
      if (_mode == "3d") {
        mode = Mode::_3D;
      } else if (_mode == "2d") {
        mode = Mode::_2D;
      } else if (_mode == "none") {
        mode = Mode::_None;
      } else {
        ENGINE_ASSERT(false, "invalid input mode!");
      }
    }
};
}

template <>
inline void nl::SceneManager::on_component_added(entt::entity e, components::CInput& c) {
  c.set_mode();

  m_registry.replace<components::CInput>(e, c);
}