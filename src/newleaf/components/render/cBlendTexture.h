#pragma once

#include <map>
#include <string>

#include <entt/entt.hpp>

#include "../../application/application.h"
#include "../../assets/asset_handle.h"
#include "../../assets/assets_manager.h"
#include "../../assets/texture.h"
#include "../../logging/log_manager.h"
#include "../../scene/scene_manager.h"
#include "../../utils/map_json_serializer.h"
#include "../../utils/numeric_comparator.h"

namespace nl {

struct CBlendTexture {
    std::string uuid;
    AssetHandle<Texture> handle;
    float blend_factor{};
    bool repeat{};

    CBlendTexture() = default;
    explicit CBlendTexture(std::string&& uuid, float bf, bool r) : uuid(std::move(uuid)), blend_factor(bf), repeat(r) {}

    void print() const {
      ENGINE_BACKTRACE("\t\tuuid: {0}\n\t\t\t\t\t\tpath: {1}\n\t\t\t\t\t\tblend_factor: {2}\n\t\t\t\t\t\trepeat: {3}",
                       uuid, handle.get() ? handle.get()->get_path() : "undefined", blend_factor, repeat);
    }

    std::map<std::string, std::string, NumericComparator> to_map() const {
      std::map<std::string, std::string, NumericComparator> info;
      info["uuid"] = uuid;
      info["blend_factor"] = std::to_string(blend_factor);
      info["texture_0"] = handle.is_valid() ? map_to_json(handle.get()->to_map()) : "undefined";
      info["repeat"] = repeat ? "true" : "false";

      return info;
    }

    void set_texture() {
      ENGINE_ASSERT(!uuid.empty(), "uuid for blend texture is empty");
      const auto& assets_manager = Application::get().get_assets_manager();
      handle = assets_manager.get<Texture>(uuid);
    }
};
}

template <> inline void nl::SceneManager::on_component_added(CBlendTexture& c) { c.set_texture(); }