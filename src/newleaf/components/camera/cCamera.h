#pragma once

#include <map>
#include <string>

#include <entt/entt.hpp>
#define GLM_FORCE_CTOR_INIT
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "../../logging/log_manager.h"
#include "../../scene/scene_manager.h"
#include "../../utils/assert.h"
#include "../../utils/numeric_comparator.h"

namespace nl {

struct CCamera {
    enum class CameraType { perspective, orthographic };

    enum class AspectRatio {
      _16_9,
      _4_3,
    };

    enum class Mode { none, _3d, _2d };

    std::string _type = "perspective";
    CameraType type;
    std::string _aspect_ratio = "16:9";
    AspectRatio aspect_ratio;
    std::string _mode;
    Mode mode;
    float aspect_ratio_value = 16.f / 9.f;
    float fov = 90.f;
    float zoom_factor = 1.f;
    float zoom_min = 1.f;
    float zoom_max = 4.f;
    float near_clip = 0.1f;
    float far_clip = 1000.f;
    glm::mat4 view{};
    glm::mat4 projection{};
    float right_angle{};
    float up_angle{};

    CCamera() = default;
    explicit CCamera(std::string&& t, std::string&& ar, std::string&& m,
                     float f, float zf, float zm, float nc, float fc)
      : _type(std::move(t)), _aspect_ratio(std::move(ar)), _mode(std::move(m)),
        fov(f), zoom_factor(zf), zoom_min(zm), zoom_max(zm), near_clip(nc),
        far_clip(fc) {}

    void print() const {
      ENGINE_BACKTRACE(
        "\t\ttype: {0}\n\t\t\t\t\t\taspect_ratio: {1}\n\t\t\t\t\t\tmode: "
        "{2}\n\t\t\t\t\t\tfov: {3}\n\t\t\t\t\t\tzoom_factor: "
        "{4}\n\t\t\t\t\t\tzoom_min: {5}\n\t\t\t\t\t\tzoom_max: "
        "{6}\n\t\t\t\t\t\tnear_clip: {7}\n\t\t\t\t\t\tfar_clip: {8}",
        _type, _aspect_ratio, _mode, fov, zoom_factor, zoom_min, zoom_max,
        near_clip, far_clip);
    }

    std::map<std::string, std::string, NumericComparator> to_map() const {
      std::map<std::string, std::string, NumericComparator> info;
      info["type"] = _type;
      info["aspect_ratio"] = _aspect_ratio;
      info["mode"] = _mode;
      info["fov"] = std::to_string(fov);
      info["zoom_factor"] = std::to_string(zoom_factor);
      info["zoom_min"] = std::to_string(zoom_min);
      info["zoom_max"] = std::to_string(zoom_max);
      info["near_clip"] = std::to_string(near_clip);
      info["far_clip"] = std::to_string(far_clip);

      return info;
    }

    void set_camera_type() {
      if (_type == "perspective") {
        type = CameraType::perspective;
      } else if (_type == "orthographic") {
        type = CameraType::orthographic;
      } else {
        ENGINE_ASSERT(false, "invalid camera type!");
      }
    }

    void set_aspect_ratio() {
      if (_aspect_ratio == "16:9") {
        aspect_ratio = AspectRatio::_16_9;
        aspect_ratio_value = 16.f / 9.f;
      } else if (_aspect_ratio == "4:3") {
        aspect_ratio = AspectRatio::_4_3;
        aspect_ratio_value = 4.f / 3.f;
      } else {
        ENGINE_ASSERT(false, "invalid camera aspect ratio!");
      }
    }

    void set_mode() {
      if (_mode == "3d") {
        mode = Mode::_3d;
      } else if (_mode == "2d") {
        mode = Mode::_2d;
      } else if (_mode == "none") {
        mode = Mode::none;
      } else {
        ENGINE_ASSERT(false, "invalid camera mode!");
      }
    }

    void calculate_projection() {
      if (type == CameraType::perspective) {
        zoom_factor = std::clamp(zoom_factor, zoom_min, zoom_max);
        projection = glm::perspective(glm::radians(fov * 1.f / zoom_factor),
                                      aspect_ratio_value, near_clip, far_clip);
      } else if (type == CameraType::orthographic) {
        projection = glm::ortho(-aspect_ratio_value * zoom_factor,
                                aspect_ratio_value * zoom_factor, -zoom_factor,
                                zoom_factor, near_clip, far_clip);
      }
    }

    void calculate_view(const glm::vec3& position, const glm::quat& rotation) {
      glm::mat4 rotated = glm::mat4_cast(glm::conjugate(rotation));
      glm::mat4 translated = glm::translate(rotated, -position);
      view = std::move(translated);
    }
};
}

template <>
inline void nl::SceneManager::on_component_added(entt::entity e, CCamera& c) {
  c.set_camera_type();
  c.set_aspect_ratio();
  c.set_mode();
  c.calculate_projection();

  m_registry.replace<CCamera>(e, c);
}