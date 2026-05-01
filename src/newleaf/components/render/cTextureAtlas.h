#pragma once

#include <map>
#include <string>

#include <entt/entt.hpp>

#include "../../assets/asset_handle.h"
#include "../../assets/texture.h"
#include "../../logging/log_manager.h"
#include "../../utils/map_json_serializer.h"
#include "../../utils/numeric_comparator.h"

namespace nl {

struct CTextureAtlas {
    std::string uuid;
    AssetHandle<Texture> handle;
    uint32_t rows{};
    uint32_t index{};

    CTextureAtlas() = default;
    explicit CTextureAtlas(std::string&& uuid, uint32_t r, uint32_t i) : uuid(std::move(uuid)), rows(r), index(i) {}

    void print() const {
      ENGINE_BACKTRACE("\t\tuuid: {0}\n\t\t\t\t\t\trows: {1}\n\t\t\t\t\t\tindex: {2}", uuid, rows, index);
    }

    std::map<std::string, std::string, NumericComparator> to_map() const {
      std::map<std::string, std::string, NumericComparator> info;
      info["uuid"] = uuid;
      info["rows"] = std::to_string(rows);
      info["index"] = std::to_string(index);
      info["texture_0"] = handle.is_valid() ? map_to_json(handle.get()->to_map()) : "undefined";
      return info;
    }

    void set_texture() {
      ENGINE_ASSERT(!uuid.empty(), "uuid for texture atlas is empty");
      const auto& assets_manager = Application::get().get_assets_manager();
      handle = assets_manager.get<Texture>(uuid);
    }
};
}

template <> inline void nl::SceneManager::on_component_added(CTextureAtlas& c) { c.set_texture(); }