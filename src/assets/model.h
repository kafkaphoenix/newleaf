#pragma once

#include <assimp/scene.h>

#include "asset.h"
#include "assets/texture.h"
#include "pch.h"
#include "scene/components/graphics/cMaterial.h"
#include "scene/components/graphics/cMesh.h"
#include "utils/numeric_comparator.h"

namespace potatoengine::assets {

class Model : public Asset {
  public:
    Model(std::filesystem::path&& fp,
          std::optional<bool> gammaCorrection = std::nullopt);

    virtual const std::map<std::string, std::string, NumericComparator>&
    to_map() override final;
    const std::map<std::string, std::string, NumericComparator>&
    get_loaded_texture_info(std::string_view textureID);

    std::vector<CMesh>& get_meshes() { return m_meshes; }
    std::vector<CMaterial>& get_materials() { return m_materials; }

    virtual bool operator==(const Asset& other) const override final;

  private:
    std::string m_path;
    std::string m_directory;
    std::vector<CMesh> m_meshes;
    std::vector<CMaterial> m_materials;
    std::vector<std::shared_ptr<Texture>> m_loaded_textures;

    std::map<std::string, std::string, NumericComparator> m_info;
    std::map<std::string, std::map<std::string, std::string, NumericComparator>,
             NumericComparator>
      m_loaded_texture_info;

    void process_node(aiNode* node, const aiScene* scene);
    CMesh process_mesh(aiMesh* mesh, const aiScene* scene);
    std::vector<std::shared_ptr<Texture>>
    load_material_textures(aiMaterial* mat, aiTextureType t, std::string type);
    CMaterial load_material(aiMaterial* mat);
};

}
