
#pragma once

#include <entt/entt.hpp>
#include <map>
#include <memory>
#include <string>

#include "../../graphics/framebuffer.h"
#include "../../graphics/shader_program.h"
#include "../../logging/log_manager.h"
#include "../../utils/assert.h"
#include "../../utils/numeric_comparator.h"

namespace nl {

struct CFBO {
    enum class Mode {
      Normal,
      Inverse,
      Greyscale,
      Blur,
      Edge,
      Sharpen,
      NightVision,
      Emboss
    };

    std::string fbo;
    std::string _mode;
    Mode mode;
    std::string _attachment;
    uint32_t attachment{};
    uint32_t width{};
    uint32_t height{};

    CFBO() = default;
    explicit CFBO(std::string&& fbo, Mode m, std::string&& attachment,
                  uint32_t w, uint32_t h)
      : fbo(std::move(fbo)), mode(m), _attachment(std::move(attachment)),
        width(w), height(h) {}

    void print() const {
      ENGINE_BACKTRACE(
        "\t\tfbo: {0}\n\t\t\t\t\t\tmode: {1}\n\t\t\t\t\t\tattachment: "
        "{2}\n\t\t\t\t\t\twidth: {3}\n\t\t\t\t\t\theight: {4}",
        fbo, _mode, _attachment, width, height);
    }

    std::map<std::string, std::string, NumericComparator> to_map() const {
      std::map<std::string, std::string, NumericComparator> info;
      info["fbo"] = fbo;
      info["mode"] = _mode;
      info["attachment"] = _attachment;
      info["width"] = std::to_string(width);
      info["height"] = std::to_string(height);

      return info;
    }

    void set_attachment() {
      if (_attachment == "depth_texture") {
        attachment = FBO::DEPTH_TEXTURE;
      } else if (_attachment == "depth_renderbuffer") {
        attachment = FBO::DEPTH_RENDERBUFFER;
      } else if (_attachment == "stencil_renderbuffer") {
        attachment = FBO::STENCIL_RENDERBUFFER;
      } else if (_attachment == "depth_stencil_renderbuffer") {
        attachment = FBO::DEPTH_STENCIL_RENDERBUFFER;
      } else {
        ENGINE_ASSERT(false, "unknown fbo attachment: {}", _attachment);
      }
    }

    void set_mode() {
      if (_mode == "normal") {
        mode = Mode::Normal;
      } else if (_mode == "inverse") {
        mode = Mode::Inverse;
      } else if (_mode == "greyscale") {
        mode = Mode::Greyscale;
      } else if (_mode == "blur") {
        mode = Mode::Blur;
      } else if (_mode == "edge") {
        mode = Mode::Edge;
      } else if (_mode == "sharpen") {
        mode = Mode::Sharpen;
      } else if (_mode == "night_vision") {
        mode = Mode::NightVision;
      } else if (_mode == "emboss") {
        mode = Mode::Emboss;
      } else {
        ENGINE_ASSERT(false, "unknown fbo mode {}", _mode);
      }
    }

    void setup_properties(const std::unique_ptr<ShaderProgram>& sp) {
      sp->reset_active_uniforms();
      sp->use();
      if (mode == Mode::Normal) {
        sp->set_float("mode", 0.f);
      } else if (mode == Mode::Inverse) {
        sp->set_float("mode", 1.f);
      } else if (mode == Mode::Greyscale) {
        sp->set_float("mode", 2.f);
      } else if (mode == Mode::Blur) {
        sp->set_float("mode", 3.f);
      } else if (mode == Mode::Edge) {
        sp->set_float("mode", 4.f);
      } else if (mode == Mode::Sharpen) {
        sp->set_float("mode", 5.f);
      } else if (mode == Mode::NightVision) {
        sp->set_float("mode", 6.f);
      } else if (mode == Mode::Emboss) {
        sp->set_float("mode", 7.f);
      }
      sp->unuse();
    }
};
}

template <>
inline void nl::SceneManager::on_component_added(entt::entity e, CFBO& c) {
  c.set_mode();
  c.set_attachment();

  m_registry.replace<CFBO>(e, c);
}