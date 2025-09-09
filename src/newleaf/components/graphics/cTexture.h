#pragma once

#include <format>
#include <map>
#include <string>
#include <vector>

#include <entt/entt.hpp>

#include "../../application/application.h"
#include "../../assets/assets_manager.h"
#include "../../assets/texture.h"
#include "../../logging/log_manager.h"
#include "../../scene/scene_manager.h"
#include "../../utils/assert.h"
#include "../../utils/map_json_serializer.h"
#include "../../utils/numeric_comparator.h"

namespace nl {

struct CTexture {
    std::vector<std::string> paths;
    std::vector<std::shared_ptr<Texture>> textures;

    CTexture() = default;
    explicit CTexture(std::vector<std::string>&& p) : paths(std::move(p)) {}

    void print() const {
      std::string texture_paths;
      if (paths.size() == 0) {
        texture_paths = "\n\t\t\t\t\t\t undefined texture paths";
      } else {
        for (std::string_view path : paths) {
          texture_paths += std::format("\n\t\t\t\t\t\t {}", path);
        }
      }
      ENGINE_BACKTRACE("\t\ttextures: {0}", texture_paths);
    }

    std::map<std::string, std::string, NumericComparator> to_map() const {
      std::map<std::string, std::string, NumericComparator> info;
      for (uint32_t i = 0; i < textures.size(); ++i) {
        info["texture_" + std::to_string(i)] = get_texture_info(i);
      }

      return info;
    }

    std::string get_texture_info(uint32_t index) const { return map_to_json(textures.at(index)->to_map()); }

    void set_textures() {
      if (paths.size() == 0) {
        return;
      }
      const auto& assets_manager = Application::get().get_assets_manager();

      textures.reserve(paths.size());
      for (std::string_view path : paths) {
        textures.emplace_back(assets_manager.get<Texture>(path));
      }
    }

    void reload_textures(std::vector<std::string>&& p) {
      ENGINE_ASSERT(p not_eq paths, "texture paths are the same");
      paths = std::move(p);
      textures.clear();
      set_textures();
    }
};
}

template <> inline void nl::SceneManager::on_component_added(entt::entity e, CTexture& c) {
  c.set_textures();

  m_registry.replace<CTexture>(e, c);
}