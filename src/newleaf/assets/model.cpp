#include "model.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#include <memory>

#include "../application/application.h"
#include "../graphics/buffer.h"
#include "../graphics/vao.h"
#include "../utils/assert.h"

namespace nl {

Model::Model(std::filesystem::path fp, std::filesystem::path shader_path, std::optional<bool> gamma_correction)
  : m_path(fp.string()) {
  ENGINE_ASSERT(not gamma_correction.has_value(), "gamma correction not yet implemented");

  const std::string directory = fp.parent_path().string();

  Assimp::Importer importer;
  const aiScene* scene = importer.ReadFile(
    m_path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace |
              aiProcess_ValidateDataStructure | aiProcess_JoinIdenticalVertices | aiProcess_OptimizeMeshes |
              aiProcess_OptimizeGraph | aiProcess_SplitLargeMeshes | aiProcess_FindInvalidData);

  ENGINE_ASSERT(scene and scene->mFlags not_eq AI_SCENE_FLAGS_INCOMPLETE and scene->mRootNode,
                "failed to load model {}: {}", m_path, importer.GetErrorString());

  std::vector<AssetHandle<Material>> materials;
  materials.reserve(scene->mNumMaterials);
  for (uint32_t i = 0; i < scene->mNumMaterials; ++i)
    materials.push_back(create_material(scene->mMaterials[i], directory, shader_path));

  size_t total_meshes = 0;
  std::function<void(aiNode*)> count_meshes = [&](aiNode* node) {
    total_meshes += node->mNumMeshes;
    for (uint32_t i = 0; i < node->mNumChildren; ++i)
      count_meshes(node->mChildren[i]);
  };
  count_meshes(scene->mRootNode);
  m_submeshes.reserve(total_meshes);

  process_node(scene->mRootNode, scene->mMeshes, materials);
}

void Model::process_node(aiNode* node, aiMesh** meshes, const std::vector<AssetHandle<Material>>& materials) {
  for (uint32_t i = 0; i < node->mNumMeshes; ++i) {
    aiMesh* mesh = meshes[node->mMeshes[i]];
    m_submeshes.push_back(SubMesh{create_mesh(mesh), materials.at(mesh->mMaterialIndex)});
  }
  for (uint32_t i = 0; i < node->mNumChildren; ++i)
    process_node(node->mChildren[i], meshes, materials);
}

std::unique_ptr<CMesh> Model::create_mesh(aiMesh* mesh) {
  std::vector<ModelVertex> vertices;
  std::vector<uint32_t> indices;
  vertices.reserve(mesh->mNumVertices);
  indices.reserve(mesh->mNumFaces * 3);

  for (uint32_t i = 0; i < mesh->mNumVertices; ++i) {
    ModelVertex vertex{};
    const auto& position = mesh->mVertices[i];
    vertex.position = glm::vec3(position.x, position.y, position.z);
    if (mesh->HasNormals()) {
      const auto& normal_vector = mesh->mNormals[i];
      vertex.normal = glm::vec3(normal_vector.x, normal_vector.y, normal_vector.z);
    }
    if (mesh->mTextureCoords[0]) {
      const auto& texture_coords = mesh->mTextureCoords[0][i];
      vertex.texture_coords = glm::vec2(texture_coords.x, texture_coords.y);
      const auto& tangent_vector = mesh->mTangents[i];
      vertex.tangent = glm::vec3(tangent_vector.x, tangent_vector.y, tangent_vector.z);
      const auto& bitangent_vector = mesh->mBitangents[i];
      vertex.bitangent = glm::vec3(bitangent_vector.x, bitangent_vector.y, bitangent_vector.z);
    }
    // TODO: bone ids and weights for skeletal animation
    vertices.emplace_back(vertex);
  }

  for (uint32_t i = 0; i < mesh->mNumFaces; ++i) {
    const aiFace& face = mesh->mFaces[i];
    for (uint32_t j = 0; j < face.mNumIndices; ++j)
      indices.emplace_back(face.mIndices[j]);
  }

  std::unique_ptr<VAO> vao = VAO::create();
  vao->attach_vertex(VBO::create(vertices), VAO::VertexType::Model);
  vao->set_index(IBO::create(indices));
  return std::make_unique<CMesh>(std::move(vao));
}

AssetHandle<Material> Model::create_material(aiMaterial* mat, const std::string& directory,
                                             const std::filesystem::path& shader_path) {
  MaterialTextures textures{};
  MaterialParams params{};
  RenderState state{};
  aiColor3D color(0.f, 0.f, 0.f);
  float shininess{};

  mat->Get(AI_MATKEY_COLOR_DIFFUSE, color);
  params.base_color_factor = glm::vec4(color.r, color.g, color.b, 1.0f);
  mat->Get(AI_MATKEY_COLOR_EMISSIVE, color);
  params.emissive_factor = glm::vec3(color.r, color.g, color.b);
  mat->Get(AI_MATKEY_SHININESS, shininess);
  params.metallic_factor = shininess;
  params.roughness_factor = 1.0f - shininess;

  auto& assets_manager = Application::get().get_assets_manager();
  aiString source;

  auto load_tex = [&](aiTextureType type, const std::string& role, AssetHandle<Texture>& dst) {
    if (mat->GetTextureCount(type) == 0)
      return;
    mat->GetTexture(type, 0, &source);
    std::string path = directory + "/" + source.C_Str();
    dst = assets_manager.get_or_load<Texture>(path, path);
    auto it = std::find_if(m_loaded_textures.begin(), m_loaded_textures.end(),
                           [&](const AssetHandle<Texture>& t) { return t.uuid() == dst.uuid(); });
    if (it == m_loaded_textures.end())
      m_loaded_textures.emplace_back(dst);
  };

  load_tex(aiTextureType_DIFFUSE, "base_color", textures.base_color);
  load_tex(aiTextureType_NORMALS, "normal", textures.normal);
  load_tex(aiTextureType_EMISSIVE, "emissive", textures.emissive);
  load_tex(aiTextureType_SPECULAR, "metallic_roughness", textures.metallic_roughness);
  load_tex(aiTextureType_AMBIENT, "occlusion", textures.occlusion);

  const std::string shader_id = shader_path.string();
  AssetHandle<Shader> shader = assets_manager.get_or_load<Shader>(shader_id, std::string(shader_id), std::string(shader_id));

  aiString mat_name;
  mat->Get(AI_MATKEY_NAME, mat_name);
  std::string material_name = mat_name.length > 0 ? mat_name.C_Str() : "Material";
  std::string material_id = m_path + "::" + material_name;

  return assets_manager.get_or_load<Material>(material_id, material_name, shader, textures, params, state);
}

const std::map<std::string, std::string, NumericComparator>& Model::to_map() {
  if (not m_info.empty())
    return m_info;

  m_info["type"] = "Model";
  m_info["path"] = m_path;
  m_info["meshes"] = std::to_string(m_submeshes.size());
  for (uint32_t i = 0; i < m_loaded_textures.size(); ++i)
    m_info["loaded_texture_" + std::to_string(i)] = std::to_string(i);

  return m_info;
}

const std::map<std::string, std::string, NumericComparator>&
Model::get_loaded_texture_info(std::string_view texture_id) {
  std::string key(texture_id);

  if (m_texture_info.contains(key))
    return m_texture_info.at(key);

  auto texture = m_loaded_textures.at(std::stoul(key)).get();
  m_texture_info[key] = texture ? texture->to_map() : std::map<std::string, std::string, NumericComparator>{};

  return m_texture_info.at(key);
}

bool Model::operator==(const Asset& other) const {
  ENGINE_ASSERT(typeid(other) == typeid(Model), "cannot compare model with other asset type!");
  return m_path == static_cast<const Model&>(other).m_path;
}

}