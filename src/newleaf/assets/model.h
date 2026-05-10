#pragma once

#include <filesystem>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include <assimp/scene.h>

#include "../render/mesh.h"
#include "../utils/numeric_comparator.h"
#include "asset.h"
#include "asset_handle.h"
#include "material.h"
#include "texture.h"

namespace nl {

struct SubMesh {
    std::unique_ptr<Mesh> mesh;
    AssetHandle<Material> material;
};

class Model : public Asset {
  public:
    Model() = delete;
    Model(std::filesystem::path fp, AssetHandle<Shader> shader);

    const std::vector<SubMesh>& get_submeshes() const { return m_submeshes; }
    std::string_view get_path() const { return m_path; }
    const std::map<std::string, std::string, NumericComparator>& to_map() override final;
    const std::map<std::string, std::string, NumericComparator>& get_material_info(std::string_view material_id);

    bool operator==(const Asset& other) const override final;

  private:
    std::string m_path;
    AssetHandle<Shader> m_shader;
    std::vector<SubMesh> m_submeshes;
    std::map<std::string, std::string, NumericComparator> m_info;

    void process_node(aiNode* node, aiMesh** meshes, const std::vector<AssetHandle<Material>>& materials);
    std::unique_ptr<Mesh> create_mesh(aiMesh* mesh);
    AssetHandle<Material> create_material(aiMaterial* mat, const std::string& directory, uint32_t index);
};

}