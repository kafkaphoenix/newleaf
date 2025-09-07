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

struct CTextureAtlas {
    std::string path;
    std::shared_ptr<Texture> texture;
    uint32_t rows{};
    uint32_t index{};

    CTextureAtlas() = default;
    explicit CTextureAtlas(std::string&& p, uint32_t r, uint32_t i) : path(std::move(p)), rows(r), index(i) {}

    void print() const {
      ENGINE_BACKTRACE("\t\tpath: {0}\n\t\t\t\t\t\trows: {1}\n\t\t\t\t\t\tindex: {2}", path, rows, index);
    }

    std::map<std::string, std::string, NumericComparator> to_map() const {
      std::map<std::string, std::string, NumericComparator> info;
      info["path"] = path;
      info["rows"] = std::to_string(rows);
      info["index"] = std::to_string(index);
      info["texture_0"] = get_texture_info();

      return info;
    }

    std::string get_texture_info() const { return map_to_json(texture->to_map()); }

    void set_texture() {
      if (path.empty()) {
        return;
      }
      const auto& assets_manager = Application::get().get_assets_manager();

      texture = assets_manager.get<Texture>(path);
    }

    void reload_texture(std::string&& p) {
      ENGINE_ASSERT(p != path, "same texture path");
      path = std::move(p);
      texture.reset();
      set_texture();
    }
};
}

template <> inline void nl::SceneManager::on_component_added(entt::entity e, CTextureAtlas& c) {
  c.set_texture();

  m_registry.replace<CTextureAtlas>(e, c);
}