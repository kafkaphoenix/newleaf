#pragma once

#define GLM_FORCE_CTOR_INIT

#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

#include "assets/texture.h"
#include "core/application.h"
#include "pch.h"
#include "utils/map_json_serializer.h"
#include "utils/numeric_comparator.h"

namespace nl {

struct CTexture {
    enum class DrawMode {
      COLOR,
      TEXTURE,
      TEXTURE_ATLAS,
      TEXTURES_BLEND,
      TEXTURE_ATLAS_BLEND,
      TEXTURE_BLEND_COLOR,
      TEXTURE_ATLAS_BLEND_COLOR
    };

    std::vector<std::string> paths;
    std::vector<std::shared_ptr<assets::Texture>> textures;
    glm::vec4 color{};
    float blend_factor{};
    float reflectivity{};
    float refractive_index{};
    bool enable_transparency{};
    bool enable_lighting{};
    bool enable_reflection{};
    bool enable_refraction{};
    std::string _draw_mode;
    DrawMode draw_mode;

    CTexture() = default;
    explicit CTexture(std::vector<std::string>&& fps, glm::vec4&& c, float bf,
                      float r, float ri, bool ht, bool ul, bool ur, bool uf,
                      std::string&& dm)
      : paths(std::move(fps)), color(std::move(c)), blend_factor(bf),
        reflectivity(r), refractive_index(ri), enable_transparency(ht),
        enable_lighting(ul), enable_reflection(ur), enable_refraction(uf),
        _draw_mode(dm) {}

    void print() const {
      std::string texturePaths;
      if (paths.size() == 0) {
        texturePaths = "\n\t\t\t\t\t\t none";
      } else {
        for (std::string_view path : paths) {
          texturePaths += std::format("\n\t\t\t\t\t\t {}", path);
        }
      }
      ENGINE_BACKTRACE(
        "\t\tcolor: {0}\n\t\t\t\t\t\tblend_factor: "
        "{1}\n\t\t\t\t\t\treflectivity: "
        "{2}\n\t\t\t\t\t\trefractive_index: "
        "{3}\n\t\t\t\t\t\tenable_transparency: "
        "{4}\n\t\t\t\t\t\tenable_lighting: "
        "{5}\n\t\t\t\t\t\tenable_reflection: "
        "{6}\n\t\t\t\t\t\tenable_refraction: {7}\n\t\t\t\t\t\tdraw_mode: "
        "{8}\n\t\t\t\t\t\ttextures: {9}",
        glm::to_string(color), blend_factor, reflectivity, refractive_index,
        enable_transparency, enable_lighting, enable_reflection,
        enable_refraction, _draw_mode, texturePaths);
    }

    std::map<std::string, std::string, NumericComparator> to_map() const {
      std::map<std::string, std::string, NumericComparator> info;
      info["color"] = glm::to_string(color);
      info["blend_factor"] = std::to_string(blend_factor);
      info["reflectivity"] = std::to_string(reflectivity);
      info["refractive_index"] = std::to_string(refractive_index);
      info["enable_transparency"] = enable_transparency ? "true" : "false";
      info["enable_lighting"] = enable_lighting ? "true" : "false";
      info["enable_reflection"] = enable_reflection ? "true" : "false";
      info["enable_refraction"] = enable_refraction ? "true" : "false";
      info["draw_mode"] = _draw_mode;

      for (uint32_t i = 0; i < textures.size(); ++i) {
        info["texture " + std::to_string(i)] = get_texture_info(i);
      }

      return info;
    }

    std::string get_texture_info(uint32_t index) const {
      return map_to_json(textures.at(index)->to_map());
    }

    void setDrawMode() { // TODO maybe send assets manager to this function?
      if (_draw_mode == "color") {
        draw_mode = DrawMode::COLOR;
      } else if (_draw_mode == "texture") {
        draw_mode = DrawMode::TEXTURE;
      } else if (_draw_mode == "textureAtlas") {
        draw_mode = DrawMode::TEXTURE_ATLAS;
      } else if (_draw_mode == "texturesBlend") { // blend two textures
        draw_mode = DrawMode::TEXTURES_BLEND;
      } else if (_draw_mode ==
                 "textureAtlasBlend") { // blend two textures in the atlas //
                                        // TODO: this is not implemented
        draw_mode = DrawMode::TEXTURE_ATLAS_BLEND;
      } else if (_draw_mode ==
                 "textureBlendColor") { // blend texture with a color
        draw_mode = DrawMode::TEXTURE_BLEND_COLOR;
      } else if (_draw_mode ==
                 "textureAtlasBlendColor") { // blend texture in the atlas with
                                             // a color
        draw_mode = DrawMode::TEXTURE_ATLAS_BLEND_COLOR;
      } else {
        ENGINE_ASSERT(false, "Unknown draw mode {}", _draw_mode);
      }
    }

    void setTextures() {
      if (paths.size() == 0) {
        return;
      }
      const auto& assets_manager = Application::Get().get_assets_manager();

      textures.reserve(paths.size());
      for (std::string_view path : paths) {
        textures.emplace_back(assets_manager->get<assets::Texture>(path));
      }
    }

    void reloadTextures(std::vector<std::string>&& fps) {
      ENGINE_ASSERT(fps != paths, "Texture paths are the same");
      paths = std::move(fps);
      textures.clear();
      setTextures();
    }
};
}

template <>
inline void nl::SceneManager::on_component_added(entt::entity e, CTexture& c) {
  c.setDrawMode();
  c.setTextures();

  m_registry.replace<CTexture>(e, c);
}