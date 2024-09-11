#pragma once

#include <entt/entt.hpp>
#include <map>
#include <string>
#include <vector>

#include "../../application/application.h"
#include "../../assets/model.h"
#include "../../logging/log_manager.h"
#include "../../utils/assert.h"
#include "../../utils/map_json_serializer.h"
#include "../../utils/numeric_comparator.h"
#include "cMaterial.h"
#include "cMesh.h"

namespace nl {

struct CBody {
    std::string path;
    std::vector<CMesh> meshes;
    std::vector<CMaterial> materials;

    CBody() = default;
    explicit CBody(std::string&& fp) : path(std::move(fp)) {}
    explicit CBody(std::string&& fp, std::vector<CMesh>&& m,
                   std::vector<CMaterial>&& ma)
      : path(std::move(fp)), meshes(std::move(m)), materials(std::move(ma)) {}

    void print() const {
      ENGINE_BACKTRACE("\t\tpath: {0}\n\t\t\t\t\t\tmeshes: "
                       "{1}\n\t\t\t\t\t\tmaterials: {2}",
                       path, meshes.size(), materials.size());
    }

    std::map<std::string, std::string, NumericComparator> to_map() const {
      std::map<std::string, std::string, NumericComparator> info;
      info["path"] = path;
      for (uint32_t i = 0; i < meshes.size(); ++i) {
        info["mesh_" + std::to_string(i)] = get_mesh_info(i);
      }
      for (uint32_t i = 0; i < materials.size(); ++i) {
        info["material_" + std::to_string(i)] = get_material_info(i);
      }

      return info;
    }

    std::string get_mesh_info(uint32_t index) const {
      return map_to_json(meshes.at(index).to_map());
    }

    std::string get_material_info(uint32_t index) const {
      return map_to_json(materials.at(index).to_map());
    }

    void set_mesh() {
      // TODO rethink if add if not empty here and do it as ctag but creating
      // all the fields
      // TODO support multiple models
      ENGINE_ASSERT(!path.empty(), "path for model is empty");
      const auto& assets_manager = Application::get().get_assets_manager();
      auto model =
        *assets_manager->get<Model>(path); // We need a copy of the model
      meshes = std::move(model.get_meshes());
      materials = std::move(model.get_materials());
    }

    void reload_mesh(std::string&& fp) {
      ENGINE_ASSERT(fp != path, "path for model is the same");
      path = std::move(fp);
      set_mesh();
    }
};
}

template <>
inline void nl::SceneManager::on_component_added(entt::entity e, CBody& c) {
  c.set_mesh();

  m_registry.replace<CBody>(e, c);
}