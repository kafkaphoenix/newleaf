#pragma once

#include <format>
#include <map>
#include <string>
#include <vector>

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

struct CTexture {
    std::string uuid;
    AssetHandle<Texture> handle;

    CTexture() = default;
    explicit CTexture(std::string&& uuid) : uuid(std::move(uuid)) {}

    void print() const {
      ENGINE_BACKTRACE("\t\tuuid: {0}\n\t\t\t\t\t\tpath: {1}", uuid,
                       handle.get() ? handle.get()->get_path() : "undefined");
    }

    std::map<std::string, std::string, NumericComparator> to_map() const {
      std::map<std::string, std::string, NumericComparator> info;
      info["uuid"] = uuid;
      info["texture_0"] = handle.is_valid() ? map_to_json(handle.get()->to_map()) : "undefined";
      return info;
    }

    void set_texture() {
      ENGINE_ASSERT(!uuid.empty(), "uuid for texture is empty");
      const auto& assets_manager = Application::get().get_assets_manager();
      handle = assets_manager.get<Texture>(uuid);
    }
};
}

template <> inline void nl::SceneManager::on_component_added(CTexture& c) { c.set_texture(); }