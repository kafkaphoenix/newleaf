#pragma once

#include <filesystem>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include <assimp/scene.h>

#include "../components/graphics/cMaterial.h"
#include "../components/graphics/cMesh.h"
#include "../utils/numeric_comparator.h"
#include "asset.h"
#include "asset_handle.h"
#include "texture.h"

namespace nl {

class Model : public Asset {
  public:
    Model(std::filesystem::path&& fp, std::optional<bool> gamma_correction = std::nullopt);

    virtual const std::map<std::string, std::string, NumericComparator>& to_map() override final;
    const std::map<std::string, std::string, NumericComparator>& get_loaded_texture_info(std::string_view textureID);

    std::vector<std::shared_ptr<CMesh>>& get_meshes() { return m_meshes; }
    const std::vector<std::shared_ptr<CMesh>>& get_meshes() const { return m_meshes; }
    std::vector<CMaterial>& get_materials() { return m_materials; }
    const std::vector<CMaterial>& get_materials() const { return m_materials; }

    virtual bool operator==(const Asset& other) const override final;

  private:
    std::string m_path;
    std::string m_directory;
    std::vector<std::shared_ptr<CMesh>> m_meshes;
    std::vector<CMaterial> m_materials;
    std::vector<AssetHandle<Texture>> m_loaded_textures;

    std::map<std::string, std::string, NumericComparator> m_info;
    std::map<std::string, std::map<std::string, std::string, NumericComparator>, NumericComparator>
      m_loaded_texture_info;

    void process_node(aiNode* node, aiMesh** meshes, aiMaterial** materials);
    CMesh process_mesh(aiMesh* mesh, aiMaterial* material);
    std::vector<AssetHandle<Texture>> load_material_textures(aiMaterial* mat, aiTextureType t, std::string type);
    CMaterial load_material(aiMaterial* mat);
};

}
