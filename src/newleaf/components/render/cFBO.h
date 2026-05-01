
#pragma once

#include <map>
#include <memory>
#include <string>

#include <entt/entt.hpp>

#include "../../application/application.h"
#include "../../assets/asset_handle.h"
#include "../../assets/shader.h"
#include "../../logging/log_manager.h"
#include "../../render/framebuffer.h"
#include "../../render/render_manager.h"
#include "../../scene/scene_manager.h"
#include "../../settings/settings_manager.h"
#include "../../utils/assert.h"
#include "../../utils/numeric_comparator.h"

namespace nl {

// TODO probably this component should be divided in two, one for fbo properties and other for effect properties
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

    std::string id;
    std::string _mode;
    Mode mode;
    float time{};
    std::string _attachment;
    uint32_t attachment{};
    uint32_t width{};
    uint32_t height{};

    CFBO() = default;
    explicit CFBO(std::string&& id, Mode m, float t, std::string&& attachment, uint32_t w, uint32_t h)
      : id(std::move(id)), mode(m), time(t), _attachment(std::move(attachment)), width(w), height(h) {}

    void print() const {
      ENGINE_BACKTRACE("\t\tid: {0}\n\t\t\t\t\t\tmode: {1}\n\t\t\t\t\t\ttime: {2}\n\t\t\t\t\t\tattachment: "
                       "{3}\n\t\t\t\t\t\twidth: {4}\n\t\t\t\t\t\theight: {5}",
                       id, _mode, time, _attachment, width, height);
    }

    std::map<std::string, std::string, NumericComparator> to_map() const {
      std::map<std::string, std::string, NumericComparator> info;
      info["id"] = id;
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

    void setup_properties(const AssetHandle<Shader>& shader) {
      Shader& sp = *shader.get();
      sp.bind();
      sp.set_float("time", time);
      if (mode == Mode::normal) {
        sp.set_int("mode", 0);
      } else if (mode == Mode::inverse) {
        sp.set_int("mode", 1);
      } else if (mode == Mode::greyscale) {
        sp.set_int("mode", 2);
      } else if (mode == Mode::blur) {
        sp.set_int("mode", 3);
      } else if (mode == Mode::edge) {
        sp.set_int("mode", 4);
      } else if (mode == Mode::sharpen) {
        sp.set_int("mode", 5);
      } else if (mode == Mode::nightvision) {
        sp.set_int("mode", 6);
      } else if (mode == Mode::emboss) {
        sp.set_int("mode", 7);
      } else if (mode == Mode::sepia) {
        sp.set_int("mode", 8);
      } else if (mode == Mode::comic) {
        sp.set_int("mode", 9);
      } else if (mode == Mode::one_bit_shading) {
        sp.set_int("mode", 10);
      } else if (mode == Mode::one_bit_dithered) {
        sp.set_int("mode", 11);
      } else if (mode == Mode::ps1) {
        sp.set_int("mode", 12);
      } else if (mode == Mode::cell_shader) {
        sp.set_int("mode", 13);
      } else if (mode == Mode::thermal) {
        sp.set_int("mode", 14);
      } else if (mode == Mode::cyanotype) {
        sp.set_int("mode", 15);
      } else if (mode == Mode::heatmap) {
        sp.set_int("mode", 16);
      } else if (mode == Mode::crt) {
        sp.set_int("mode", 17);
      } else if (mode == Mode::chromatic_aberration) {
        sp.set_int("mode", 18);
      } else if (mode == Mode::swirl) {
        sp.set_int("mode", 19);
      } else if (mode == Mode::horizontal_glitch) {
        sp.set_int("mode", 20);
      } else if (mode == Mode::wave) {
        sp.set_int("mode", 21);
      } else if (mode == Mode::kaleidoscope) {
        sp.set_int("mode", 22);
      } else if (mode == Mode::datamosh) {
        sp.set_int("mode", 23);
      } else if (mode == Mode::barrel_distortion) {
        sp.set_int("mode", 24);
      } else if (mode == Mode::ripple) {
        sp.set_int("mode", 25);
      } else if (mode == Mode::plasma) {
        sp.set_int("mode", 26);
      } else if (mode == Mode::pixelate) {
        sp.set_int("mode", 27);
      } else if (mode == Mode::mirror) {
        sp.set_int("mode", 28);
      } else {
        ENGINE_ASSERT(false, "unknown fbo mode {}", _mode);
      }
      sp.unbind();
    }
};
}

template <> inline void nl::SceneManager::on_component_added(CFBO& c) {
  c.set_mode();
  c.set_attachment();
  ENGINE_ASSERT(!c.id.empty(), "fbo id cannot be empty!");
  if (c.width == 0 or c.height == 0) {
    const auto& settings_manager = Application::get().get_settings_manager();
    c.width = settings_manager.window_w;
    c.height = settings_manager.window_h;
  }
  auto& render_manager = Application::get().get_render_manager();
  render_manager.add_framebuffer(std::string(c.id), c.width, c.height, c.attachment);
}