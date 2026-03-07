#pragma once

#include <filesystem>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include <assimp/scene.h>

#include "../components/graphics/Material.h"
#include "../components/graphics/cMesh.h"
#include "../utils/numeric_comparator.h"
#include "asset.h"
#include "asset_handle.h"
#include "texture.h"

namespace nl {

struct SubMesh {
    std::unique_ptr<CMesh> mesh;
    AssetHandle<Material> material;
};

class Model : public Asset {
  public:
    Model() = delete;
    Model(std::filesystem::path fp, std::filesystem::path shader_path,
          std::optional<bool> gamma_correction = std::nullopt);

    const std::vector<SubMesh>& get_submeshes() const { return m_submeshes; }

    const std::map<std::string, std::string, NumericComparator>& to_map() override final;
    const std::map<std::string, std::string, NumericComparator>& get_loaded_texture_info(std::string_view texture_id);

    bool operator==(const Asset& other) const override final;

  private:
    std::string m_path;
    std::vector<SubMesh> m_submeshes;
    std::vector<AssetHandle<Texture>> m_loaded_textures;
    std::map<std::string, std::string, NumericComparator> m_info;
    std::map<std::string, std::map<std::string, std::string, NumericComparator>, NumericComparator> m_texture_info;

    void process_node(aiNode* node, aiMesh** meshes, const std::vector<AssetHandle<Material>>& materials);
    std::unique_ptr<CMesh> create_mesh(aiMesh* mesh);
    AssetHandle<Material> create_material(aiMaterial* mat, const std::string& directory,
                                          const std::filesystem::path& shader_path);
};

}