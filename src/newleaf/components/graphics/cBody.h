#pragma once

#include <map>
#include <string>
#include <vector>

#include <entt/entt.hpp>

#include "../../application/application.h"
#include "../../assets/asset_handle.h"
#include "../../assets/model.h"
#include "../../logging/log_manager.h"
#include "../../scene/scene_manager.h"
#include "../../utils/assert.h"
#include "../../utils/map_json_serializer.h"
#include "../../utils/numeric_comparator.h"
#include "cMesh.h"

namespace nl {

struct CBody {
    std::string path;
    AssetHandle<Model> handle;
    std::vector<CMesh*> meshes;

    CBody() = default;
    explicit CBody(std::string&& fp) : path(std::move(fp)) {}
    explicit CBody(std::string&& fp, std::vector<CMesh*>&& m) : path(std::move(fp)), meshes(std::move(m)) {}

    void print() const {
      ENGINE_BACKTRACE("\t\tpath: {0}\n\t\t\t\t\t\tmeshes: "
                       "{1}",
                       path, meshes.size());
    }

    std::map<std::string, std::string, NumericComparator> to_map() const {
      std::map<std::string, std::string, NumericComparator> info;
      info["path"] = path;
      for (uint32_t i = 0; i < meshes.size(); ++i) {
        info["mesh_" + std::to_string(i)] = get_mesh_info(i);
      }

      return info;
    }

    std::string get_mesh_info(uint32_t index) const { return map_to_json(meshes.at(index)->to_map()); }

    void set_mesh() {
      ENGINE_ASSERT(!path.empty(), "path for model is empty");
      const auto& assets_manager = Application::get().get_assets_manager();
      handle = assets_manager.get<Model>(path);
      Model& model = *handle.get();
      meshes.clear();
      meshes.reserve(model.get_submeshes().size());
      for (auto& submesh : model.get_submeshes()) {
        meshes.emplace_back(submesh.mesh.get());
      }
    }

    void reload_mesh(std::string&& fp) {
      ENGINE_ASSERT(fp not_eq path, "path for model is the same");
      path = std::move(fp);
      handle = AssetHandle<Model>();
      set_mesh();
    }
};
}

template <> inline void nl::SceneManager::on_component_added(CBody& c) { c.set_mesh(); }