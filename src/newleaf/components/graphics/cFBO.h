
#pragma once

#include <map>
#include <memory>
#include <string>

#include <entt/entt.hpp>

#include "../../graphics/framebuffer.h"
#include "../../graphics/render_manager.h"
#include "../../graphics/shader_program.h"
#include "../../logging/log_manager.h"
#include "../../scene/scene_manager.h"
#include "../../utils/assert.h"
#include "../../utils/numeric_comparator.h"

namespace nl {

struct CFBO {
    enum class Mode {
      normal,
      inverse,
      greyscale,
      blur,
      edge,
      sharpen,
      nightvision,
      emboss,
      sepia,
      comic,
      one_bit_shading,
      one_bit_dithered,
      ps1,
      cell_shader,
      thermal,
      cyanotype,
      heatmap,
      crt,
      chromatic_aberration,
      swirl,
      horizontal_glitch,
      wave,
      kaleidoscope,
      datamosh,
      barrel_distortion,
      ripple,
      plasma,
      pixelate,
      mirror
    };

    std::string fbo;
    std::string _mode;
    Mode mode;
    float time{};
    std::string _attachment;
    uint32_t attachment{};
    uint32_t width{};
    uint32_t height{};

    CFBO() = default;
    explicit CFBO(std::string&& fbo, Mode m, float t, std::string&& attachment, uint32_t w, uint32_t h)
      : fbo(std::move(fbo)), mode(m), time(t), _attachment(std::move(attachment)), width(w), height(h) {}

    void print() const {
      ENGINE_BACKTRACE("\t\tfbo: {0}\n\t\t\t\t\t\tmode: {1}\n\t\t\t\t\t\ttime: {2}\n\t\t\t\t\t\tattachment: "
                       "{3}\n\t\t\t\t\t\twidth: {4}\n\t\t\t\t\t\theight: {5}",
                       fbo, _mode, time, _attachment, width, height);
    }

    std::map<std::string, std::string, NumericComparator> to_map() const {
      std::map<std::string, std::string, NumericComparator> info;
      info["fbo"] = fbo;
      info["mode"] = _mode;
      info["time"] = std::to_string(time);
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
        mode = Mode::normal;
      } else if (_mode == "inverse") {
        mode = Mode::inverse;
      } else if (_mode == "greyscale") {
        mode = Mode::greyscale;
      } else if (_mode == "blur") {
        mode = Mode::blur;
      } else if (_mode == "edge") {
        mode = Mode::edge;
      } else if (_mode == "sharpen") {
        mode = Mode::sharpen;
      } else if (_mode == "nightvision") {
        mode = Mode::nightvision;
      } else if (_mode == "emboss") {
        mode = Mode::emboss;
      } else if (_mode == "sepia") {
        mode = Mode::sepia;
      } else if (_mode == "comic") {
        mode = Mode::comic;
      } else if (_mode == "one_bit_shading") {
        mode = Mode::one_bit_shading;
      } else if (_mode == "one_bit_dithered") {
        mode = Mode::one_bit_dithered;
      } else if (_mode == "ps1") {
        mode = Mode::ps1;
      } else if (_mode == "cell_shader") {
        mode = Mode::cell_shader;
      } else if (_mode == "thermal") {
        mode = Mode::thermal;
      } else if (_mode == "cyanotype") {
        mode = Mode::cyanotype;
      } else if (_mode == "heatmap") {
        mode = Mode::heatmap;
      } else if (_mode == "crt") {
        mode = Mode::crt;
      } else if (_mode == "chromatic_aberration") {
        mode = Mode::chromatic_aberration;
      } else if (_mode == "swirl") {
        mode = Mode::swirl;
      } else if (_mode == "horizontal_glitch") {
        mode = Mode::horizontal_glitch;
      } else if (_mode == "wave") {
        mode = Mode::wave;
      } else if (_mode == "kaleidoscope") {
        mode = Mode::kaleidoscope;
      } else if (_mode == "datamosh") {
        mode = Mode::datamosh;
      } else if (_mode == "barrel_distortion") {
        mode = Mode::barrel_distortion;
      } else if (_mode == "ripple") {
        mode = Mode::ripple;
      } else if (_mode == "plasma") {
        mode = Mode::plasma;
      } else if (_mode == "pixelate") {
        mode = Mode::pixelate;
      } else if (_mode == "mirror") {
        mode = Mode::mirror;
      } else {
        ENGINE_ASSERT(false, "unknown fbo mode {}", _mode);
      }
    }

    void setup_properties(ShaderProgram& sp) {
      sp.reset_active_uniforms();
      sp.use();
      if (mode == Mode::normal) {
        sp.set_float("mode", 0.f);
      } else if (mode == Mode::inverse) {
        sp.set_float("mode", 1.f);
      } else if (mode == Mode::greyscale) {
        sp.set_float("mode", 2.f);
      } else if (mode == Mode::blur) {
        sp.set_float("mode", 3.f);
      } else if (mode == Mode::edge) {
        sp.set_float("mode", 4.f);
      } else if (mode == Mode::sharpen) {
        sp.set_float("mode", 5.f);
      } else if (mode == Mode::nightvision) {
        sp.set_float("mode", 6.f);
      } else if (mode == Mode::emboss) {
        sp.set_float("mode", 7.f);
      } else if (mode == Mode::sepia) {
        sp.set_float("mode", 8.f);
      } else if (mode == Mode::comic) {
        sp.set_float("mode", 9.f);
      } else if (mode == Mode::one_bit_shading) {
        sp.set_float("mode", 10.f);
      } else if (mode == Mode::one_bit_dithered) {
        sp.set_float("mode", 11.f);
      } else if (mode == Mode::ps1) {
        sp.set_float("mode", 12.f);
      } else if (mode == Mode::cell_shader) {
        sp.set_float("mode", 13.f);
      } else if (mode == Mode::thermal) {
        sp.set_float("mode", 14.f);
      } else if (mode == Mode::cyanotype) {
        sp.set_float("mode", 15.f);
      } else if (mode == Mode::heatmap) {
        sp.set_float("mode", 16.f);
      } else if (mode == Mode::crt) {
        sp.set_float("mode", 17.f);
      } else if (mode == Mode::chromatic_aberration) {
        sp.set_float("mode", 18.f);
      } else if (mode == Mode::swirl) {
        sp.set_float("mode", 19.f);
      } else if (mode == Mode::horizontal_glitch) {
        sp.set_float("mode", 20.f);
      } else if (mode == Mode::wave) {
        sp.set_float("mode", 21.f);
      } else if (mode == Mode::kaleidoscope) {
        sp.set_float("mode", 22.f);
      } else if (mode == Mode::datamosh) {
        sp.set_float("mode", 23.f);
      } else if (mode == Mode::barrel_distortion) {
        sp.set_float("mode", 24.f);
      } else if (mode == Mode::ripple) {
        sp.set_float("mode", 25.f);
      } else if (mode == Mode::plasma) {
        sp.set_float("mode", 26.f);
      } else if (mode == Mode::pixelate) {
        sp.set_float("mode", 27.f);
      } else if (mode == Mode::mirror) {
        sp.set_float("mode", 28.f);
      }
      sp.unuse();
    }
};
}

template <> inline void nl::SceneManager::on_component_added(entt::entity e, CFBO& c) {
  c.set_mode();
  c.set_attachment();
  auto& render_manager = Application::get().get_render_manager();
  render_manager.add_framebuffer(std::string(c.fbo), c.width, c.height, c.attachment);

  m_registry.replace<CFBO>(e, c);
}