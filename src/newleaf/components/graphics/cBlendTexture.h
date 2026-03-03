#pragma once

#include <map>
#include <string>

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

struct CBlendTexture {
    std::string path;
    std::shared_ptr<Texture> texture;
    float blend_factor{};
    bool repeat{};

    CBlendTexture() = default;
    explicit CBlendTexture(std::string&& p, float b) : path(std::move(p)), blend_factor(b) {}

    void print() const {
      ENGINE_BACKTRACE("\t\tpath: {0}\n\t\t\t\t\t\tblend_factor: {1}\n\t\t\t\t\t\trepeat: {2}", path, blend_factor,
                       repeat);
    }

    std::map<std::string, std::string, NumericComparator> to_map() const {
      std::map<std::string, std::string, NumericComparator> info;
      info["path"] = path;
      info["blend_factor"] = std::to_string(blend_factor);
      info["texture_0"] = get_texture_info();
      info["repeat"] = repeat ? "true" : "false";

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
      ENGINE_ASSERT(p not_eq path, "same texture path");
      path = std::move(p);
      texture.reset();
      set_texture();
    }
};
}

template <> inline void nl::SceneManager::on_component_added(CBlendTexture& c) { c.set_texture(); }