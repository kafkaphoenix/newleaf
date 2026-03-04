#include "model.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#include <memory>

#include "../application/application.h"
#include "../graphics/buffer.h"
#include "../graphics/vao.h"
#include "../settings/settings_manager.h"
#include "../utils/assert.h"

namespace nl {

Model::Model(std::filesystem::path&& fp, std::optional<bool> gamma_correction)
  : m_path(std::move(fp.string())), m_directory(std::move(fp.parent_path().string())) {
  ENGINE_ASSERT(not gamma_correction.has_value(), "gamma correction not yet implemented");

  Assimp::Importer importer;
  const aiScene* scene = importer.ReadFile(
    m_path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace |
              aiProcess_ValidateDataStructure | aiProcess_JoinIdenticalVertices | aiProcess_OptimizeMeshes |
              aiProcess_OptimizeGraph | aiProcess_SplitLargeMeshes | aiProcess_FindInvalidData);

  ENGINE_ASSERT(scene and scene->mFlags not_eq AI_SCENE_FLAGS_INCOMPLETE and scene->mRootNode,
                "failed to load model {}: {}", m_path, importer.GetErrorString());

  process_node(scene->mRootNode, scene->mMeshes, scene->mMaterials);
}

void Model::process_node(aiNode* node, aiMesh** meshes, aiMaterial** materials) {
  m_meshes.reserve(node->mNumMeshes);
  for (uint32_t i = 0; i < node->mNumMeshes; ++i) {
    aiMesh* mesh = meshes[node->mMeshes[i]];
    aiMaterial* material = materials[mesh->mMaterialIndex];
    m_meshes.emplace_back(std::make_shared<CMesh>(std::move(process_mesh(mesh, material))));
  }

  for (uint32_t i = 0; i < node->mNumChildren; ++i) {
    process_node(node->mChildren[i], meshes, materials);
  }
}

// TODO each material is related to a texture, so maybe save texture info in material too
// rethink all this logic after assets manager refactor with handlers
CMesh Model::process_mesh(aiMesh* mesh, aiMaterial* material) {
  std::vector<ModelVertex> vertices{};
  std::vector<uint32_t> indices{};
  std::vector<AssetHandle<Texture>> textures;
  vertices.reserve(mesh->mNumVertices);

  for (uint32_t i = 0; i < mesh->mNumVertices; ++i) {
    ModelVertex vertex{};
    const auto& position = mesh->mVertices[i]; // assimp vector does not directly convert to glm vec3
    vertex.position = glm::vec3(position.x, position.y, position.z);

    if (mesh->HasNormals()) {
      const auto& normal_vector = mesh->mNormals[i];
      vertex.normal = glm::vec3(normal_vector.x, normal_vector.y, normal_vector.z);
    }

    if (mesh->mTextureCoords[0]) {
      // a vertex can contain up to 8 different texture coordinates. We thus
      // make the assumption that we won't use models where a vertex can have
      // multiple texture coordinates so we always take the first set (0).
      const auto& texture_coords = mesh->mTextureCoords[0][i];
      vertex.texture_coords = glm::vec2(texture_coords.x, texture_coords.y);

      const auto& tangent_vector = mesh->mTangents[i];
      vertex.tangent = glm::vec3(tangent_vector.x, tangent_vector.y, tangent_vector.z);

      const auto& bitangent_vector = mesh->mBitangents[i];
      vertex.bitangent = glm::vec3(bitangent_vector.x, bitangent_vector.y, bitangent_vector.z);
    }

    if (mesh->HasBones()) {
      // TODO bones
    }

    if (mesh->HasVertexColors(0)) {
      // TODO vertex colors maybe i dont need? or think if adding color component a better way
      // or sending here an entity
    }

    if (mesh->mAABB.mMin not_eq aiVector3D(0.f, 0.f, 0.f) and mesh->mAABB.mMax not_eq aiVector3D(0.f, 0.f, 0.f)) {
      // TODO aabb with models
    }

    if (mesh->HasFaces()) {
      // TODO faces
    }

    vertices.emplace_back(std::move(vertex));
  }

  for (uint32_t i = 0; i < mesh->mNumFaces; ++i) {
    aiFace face = mesh->mFaces[i];
    indices.reserve(face.mNumIndices);
    for (uint32_t j = 0; j < face.mNumIndices; ++j) {
      indices.emplace_back(face.mIndices[j]);
    }
  }

  CMaterial material_data = load_material(material);

  // n is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER.
  // diffuse: texture_diffuse_n
  // specular: texture_specular_n
  // normal: texture_normal_n
  // height: texture_height_n
  auto load_and_insert_textures = [&](aiTextureType t, std::string type) {
    std::vector<AssetHandle<Texture>> loaded_textures = load_material_textures(material, t, type);
    textures.insert(textures.end(), loaded_textures.begin(),
                    loaded_textures.end()); // Can't be emplace
  };

  load_and_insert_textures(aiTextureType_DIFFUSE, "texture_diffuse");
  load_and_insert_textures(aiTextureType_SPECULAR, "texture_specular");
  load_and_insert_textures(aiTextureType_HEIGHT, "texture_normal");
  load_and_insert_textures(aiTextureType_AMBIENT, "texture_height");

  // 0.6 is the default value for diffuse in assimp
  // TODO rethink default texture probably should be just a error
  if (textures.empty() and material_data.diffuse == glm::vec3(0.6f)) {
    std::string_view default_texture_path = Application::get().get_settings_manager().default_texture_path;
    if (not default_texture_path.empty()) {
      auto& assets_manager = Application::get().get_assets_manager();
      const auto default_handle = assets_manager.get<Texture>("default");
      if (not default_handle.is_valid()) {
        ENGINE_ERROR("default texture not found at {}, make sure the path is correct in settings",
                     default_texture_path);
      }
      textures.emplace_back(default_handle);
    }
  }
  m_materials.emplace_back(std::move(material_data));

  std::unique_ptr<VAO> vao = VAO::create();
  std::unique_ptr<VBO> vbo = VBO::create(vertices);
  std::unique_ptr<IBO> ibo = IBO::create(indices);
  vao->attach_vertex(std::move(vbo), VAO::VertexType::Model);
  vao->set_index(std::move(ibo));
  return CMesh(std::move(vao), std::move(textures));
}

std::vector<AssetHandle<Texture>> Model::load_material_textures(aiMaterial* mat, aiTextureType t, std::string type) {
  std::vector<AssetHandle<Texture>> textures;
  textures.reserve(mat->GetTextureCount(t));
  for (uint32_t i = 0; i < mat->GetTextureCount(t); ++i) {
    aiString source;
    mat->GetTexture(t, i, &source);
    std::string filename = source.C_Str();
    std::string path = m_directory + "/" + filename;

    auto& assets_manager = Application::get().get_assets_manager();
    auto handle = assets_manager.try_get<Texture>(path);
    if (!handle.is_valid()) {
      handle = assets_manager.get_or_load<Texture>(path, path, type);
    }

    textures.emplace_back(handle);
    auto loaded_it =
      std::find_if(m_loaded_textures.begin(), m_loaded_textures.end(),
                   [&](const AssetHandle<Texture>& existing) { return existing.uuid() == handle.uuid(); });
    if (loaded_it == m_loaded_textures.end()) {
      m_loaded_textures.emplace_back(handle);
    }
  }

  return textures;
}

// TODO each material is related to a texture, so maybe save texture info in material too
CMaterial Model::load_material(aiMaterial* mat) {
  // map_Ns        SHININESS    roughness
  // map_Ka        AMBIENT      ambient occlusion
  // map_Kd        DIFFUSE      albedo diffuse
  // map_Ks        SPECULAR     metallic specular
  // map_Ke        EMISSIVE     emissive
  // map_Ni        REFRACTION   ior
  // map_d         OPACITY
  // illum         ILLUMINATION
  // map_Bump      HEIGHT       height
  // map_Kn        NORMALS      normal
  // map_disp      DISPLACEMENT
  CMaterial material{};
  aiColor3D color(0.f, 0.f, 0.f);
  float shininess{};

  mat->Get(AI_MATKEY_COLOR_AMBIENT, color);
  material.ambient = glm::vec3(color.r, color.b, color.g);

  mat->Get(AI_MATKEY_COLOR_DIFFUSE, color);
  material.diffuse = glm::vec3(color.r, color.b, color.g);

  mat->Get(AI_MATKEY_COLOR_SPECULAR, color);
  material.specular = glm::vec3(color.r, color.b, color.g);

  mat->Get(AI_MATKEY_SHININESS, shininess);
  material.shininess = shininess;

  return material;
}

const std::map<std::string, std::string, NumericComparator>& Model::to_map() {
  if (not m_info.empty()) {
    return m_info;
  }

  m_info["type"] = "Model";
  m_info["path"] = m_path;
  m_info["meshes"] = std::to_string(m_meshes.size());
  m_info["materials"] = std::to_string(m_materials.size());
  for (uint32_t i = 0; i < m_loaded_textures.size(); ++i) {
    m_info["loaded_texture_" + std::to_string(i)] = std::to_string(i);
  }

  return m_info;
}

const std::map<std::string, std::string, NumericComparator>&
Model::get_loaded_texture_info(std::string_view textureID) {
  if (not m_loaded_texture_info.empty() and m_loaded_texture_info.contains(std::string(textureID))) {
    return m_loaded_texture_info.at(std::string(textureID));
  }

  auto texture = m_loaded_textures.at(std::stoi(textureID.data())).get();
  m_loaded_texture_info[std::string(textureID)] =
    texture ? texture->to_map() : std::map<std::string, std::string, NumericComparator>{};

  return m_loaded_texture_info.at(std::string(textureID));
}

bool Model::operator==(const Asset& other) const {
  if (typeid(other) not_eq typeid(Model)) {
    ENGINE_ASSERT(false, "cannot compare model with other asset type!");
  }
  const Model& other_model = static_cast<const Model&>(other);
  return m_path == other_model.m_path;
}

}
