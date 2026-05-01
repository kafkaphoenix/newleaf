#pragma once

#include <map>
#include <string>
#include <vector>

#include <entt/entt.hpp>

#include "../../application/application.h"
#include "../../assets/asset_handle.h"
#include "../../assets/model.h"
#include "../../logging/log_manager.h"
#include "../../render/mesh.h"
#include "../../scene/scene_manager.h"
#include "../../utils/assert.h"
#include "../../utils/map_json_serializer.h"
#include "../../utils/numeric_comparator.h"

namespace nl {

struct CModel {
    std::string uuid;
    AssetHandle<Model> handle;

    CModel() = default;
    explicit CModel(std::string&& uuid) : uuid(std::move(uuid)) {}

    void print() const {
      ENGINE_BACKTRACE("\t\tuuid: {0}\n\t\t\t\t\t\tpath: {1}\n\t\t\t\t\t\tmeshes: "
                       "{2}",
                       uuid, handle.get() ? handle.get()->get_path() : "undefined",
                       handle.get() ? handle.get()->get_submeshes().size() : 0);
    }

    std::map<std::string, std::string, NumericComparator> to_map() const {
      std::map<std::string, std::string, NumericComparator> info;
      info["uuid"] = uuid;
      info["path"] = handle.get() ? handle.get()->get_path() : "undefined";
      for (size_t i = 0; handle.get() and i < handle.get()->get_submeshes().size(); ++i) {
        const auto& submesh = handle.get()->get_submeshes()[i];
        info["material_" + std::to_string(i)] =
          submesh.material.is_valid() ? map_to_json(submesh.material.get()->to_map()) : "undefined";
      }

      return info;
    }

    void set_model() {
      ENGINE_ASSERT(!uuid.empty(), "uuid for model is empty");
      const auto& assets_manager = Application::get().get_assets_manager();

      handle = assets_manager.get<Model>(uuid);
    }
};
}

template <> inline void nl::SceneManager::on_component_added(CModel& c) { c.set_model(); }