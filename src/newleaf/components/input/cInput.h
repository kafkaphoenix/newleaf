#pragma once

#include <map>
#include <string>

#include <entt/entt.hpp>
#define GLM_FORCE_CTOR_INIT
#include <glm/glm.hpp>

#include "../../logging/log_manager.h"
#include "../../utils/assert.h"
#include "../../utils/numeric_comparator.h"

namespace nl {

struct CInput {
    enum class Mode { none, _3d, _2d };

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
        mode = Mode::_3d;
      } else if (_mode == "2d") {
        mode = Mode::_2d;
      } else if (_mode == "none") {
        mode = Mode::none;
      } else {
        ENGINE_ASSERT(false, "invalid input mode!");
      }
    }
};
}

template <>
inline void nl::SceneManager::on_component_added(entt::entity e, CInput& c) {
  c.set_mode();

  m_registry.replace<CInput>(e, c);
}