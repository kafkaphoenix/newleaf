#pragma once

#include <algorithm>
#include <format>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include <entt/entt.hpp>

#include "../../application/application.h"
#include "../../assets/asset_handle.h"
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
    std::vector<AssetHandle<Texture>> handles;

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
      for (uint32_t i = 0; i < handles.size(); ++i) {
        info["texture_" + std::to_string(i)] = get_texture_info(i);
      }

      return info;
    }

    std::string get_texture_info(uint32_t index) const {
      auto texture = get_texture(index);
      return texture ? map_to_json(texture->to_map()) : "undefined";
    }

    std::shared_ptr<Texture> get_texture(uint32_t index) const {
      if (index >= handles.size()) {
        return nullptr;
      }
      return handles.at(index).get();
    }

    void set_textures() {
      handles.clear();
      if (paths.size() == 0) {
        return;
      }
      const auto& assets_manager = Application::get().get_assets_manager();

      handles.reserve(paths.size());
      for (const auto& path : paths) {
        auto handle = assets_manager.get<Texture>(path);
        handles.emplace_back(handle);
      }
    }

    void reload_textures(std::vector<std::string>&& p) {
      ENGINE_ASSERT(p not_eq paths, "texture paths are the same");
      paths = std::move(p);
      handles.clear();
      set_textures();
    }
};
}

template <> inline void nl::SceneManager::on_component_added(CTexture& c) { c.set_textures(); }