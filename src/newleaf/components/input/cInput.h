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
    float mouseSensitivity = 0.1f;
    float translationSpeed = 20.f;
    float verticalSpeed = 45.f;
    float rotation_speed = 180.f;

    CInput() = default;
    explicit CInput(std::string&& m, float ms, float ts, float vs, float rs)
      : _mode(std::move(m)), mouseSensitivity(ms), translationSpeed(ts),
        verticalSpeed(vs), rotation_speed(rs) {}

    void print() const {
      ENGINE_BACKTRACE(
        "\t\tmode: {0}\n\t\t\t\t\t\tmouseSensitivity: "
        "{1}\n\t\t\t\t\t\ttranslationSpeed: {2}\n\t\t\t\t\t\tverticalSpeed: "
        "{3}\n\t\t\t\t\t\trotation_speed: {4}",
        _mode, mouseSensitivity, translationSpeed, verticalSpeed,
        rotation_speed);
    }

    std::map<std::string, std::string, NumericComparator> to_map() const {
      std::map<std::string, std::string, NumericComparator> info;
      info["mode"] = _mode;
      info["mouseSensitivity"] = std::to_string(mouseSensitivity);
      info["translationSpeed"] = std::to_string(translationSpeed);
      info["verticalSpeed"] = std::to_string(verticalSpeed);
      info["rotation_speed"] = std::to_string(rotation_speed);

      return info;
    }

    void set_mode() {
      if (_mode == "3D") {
        mode = Mode::_3D;
      } else if (_mode == "2D") {
        mode = Mode::_2D;
      } else if (_mode == "none") {
        mode = Mode::_None;
      } else {
        ENGINE_ASSERT(false, "Invalid input mode!");
      }
    }
};
}

template <>
inline void nl::SceneManager::on_component_added(entt::entity e, components::CInput& c) {
  c.set_mode();

  m_registry.replace<components::CInput>(e, c);
}