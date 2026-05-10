#include "model.h"

#include <assimp/GltfMaterial.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#include <format>
#include <memory>

#include "../application/application.h"
#include "../render/buffer.h"
#include "../render/vao.h"
#include "../utils/assert.h"
#include "assets_manager.h"
#include "shader.h"

namespace nl {

Model::Model(std::filesystem::path fp, AssetHandle<Shader> shader) : m_path(fp.string()), m_shader(std::move(shader)) {

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
    materials.push_back(create_material(scene->mMaterials[i], directory, i));

  m_submeshes.reserve(scene->mNumMeshes);

  process_node(scene->mRootNode, scene->mMeshes, materials);
}

void Model::process_node(aiNode* node, aiMesh** meshes, const std::vector<AssetHandle<Material>>& materials) {
  for (uint32_t i = 0; i < node->mNumMeshes; ++i) {
    aiMesh* mesh = meshes[node->mMeshes[i]];
    m_submeshes.emplace_back(create_mesh(mesh), materials.at(mesh->mMaterialIndex));
  }
  for (uint32_t i = 0; i < node->mNumChildren; ++i)
    process_node(node->mChildren[i], meshes, materials);
}

std::unique_ptr<Mesh> Model::create_mesh(aiMesh* mesh) {
  std::vector<ModelVertex> vertices;
  std::vector<uint32_t> indices;

  vertices.reserve(mesh->mNumVertices);
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

  indices.reserve(mesh->mNumFaces * 3);
  for (uint32_t i = 0; i < mesh->mNumFaces; ++i) {
    const aiFace& face = mesh->mFaces[i];
    for (uint32_t j = 0; j < face.mNumIndices; ++j)
      indices.emplace_back(face.mIndices[j]);
  }

  std::unique_ptr<VAO> vao = VAO::create();
  vao->attach_vertex(VBO::create(vertices), VAO::VertexType::Model);
  vao->set_index(IBO::create(indices));
  return std::make_unique<Mesh>(std::move(vao));
}

AssetHandle<Material> Model::create_material(aiMaterial* mat, const std::string& directory, uint32_t index) {
  MaterialTextures textures{};
  MaterialParams params{};
  RenderState state{};
  aiColor4D color{1.0f, 1.0f, 1.0f, 1.0f};
  aiColor3D emissive_color{0.0f, 0.0f, 0.0f};
  float metallic = 1.0f;
  float roughness = 1.0f;
  aiString alpha_mode;

  mat->Get(AI_MATKEY_BASE_COLOR, color);
  params.base_color_factor = glm::vec4(color.r, color.g, color.b, color.a);
  mat->Get(AI_MATKEY_COLOR_EMISSIVE, emissive_color);
  params.emissive_factor = glm::vec3(emissive_color.r, emissive_color.g, emissive_color.b);
  mat->Get(AI_MATKEY_METALLIC_FACTOR, metallic);
  params.metallic_factor = metallic;
  mat->Get(AI_MATKEY_ROUGHNESS_FACTOR, roughness);
  params.roughness_factor = roughness;

  if (mat->Get(AI_MATKEY_GLTF_ALPHAMODE, alpha_mode) == AI_SUCCESS) {
    std::string_view mode = alpha_mode.C_Str();

    if (mode == "MASK") {
      mat->Get(AI_MATKEY_GLTF_ALPHACUTOFF, params.alpha_cutoff);
    } else if (mode == "BLEND") {
      state.blend = true;
      state.depth_write = false;
    }
  }

  int double_sided = 0;
  if (mat->Get(AI_MATKEY_TWOSIDED, double_sided) == AI_SUCCESS) {
    state.cull = (double_sided == 0);
  }

  auto& assets_manager = Application::get().get_assets_manager();
  auto load_texture = [&](aiTextureType type) -> AssetHandle<Texture> {
    if (mat->GetTextureCount(type) == 0)
      return {};

    aiString source;
    mat->GetTexture(type, 0, &source);
    const auto path = (std::filesystem::path(directory) / source.C_Str()).lexically_normal();
    return assets_manager.get_or_load<Texture>(path.string(), std::filesystem::path(path));
  };

  auto load_first_texture = [&](std::initializer_list<aiTextureType> types) -> AssetHandle<Texture> {
    for (const auto type : types) {
      if (auto texture = load_texture(type); texture.is_valid())
        return texture;
    }
    return {};
  };

  // PBR Metallic/Roughness types first, legacy fallback for non-PBR models (e.g. OBJ)
  textures.base_color = load_first_texture({aiTextureType_BASE_COLOR, aiTextureType_DIFFUSE});
  textures.normal = load_first_texture({aiTextureType_NORMALS});
  textures.emissive = load_first_texture({aiTextureType_EMISSION_COLOR, aiTextureType_EMISSIVE});
  textures.metallic_roughness =
    load_first_texture({aiTextureType_METALNESS, aiTextureType_UNKNOWN, aiTextureType_GLTF_METALLIC_ROUGHNESS});
  textures.occlusion = load_first_texture({aiTextureType_AMBIENT_OCCLUSION, aiTextureType_LIGHTMAP});

  aiString mat_name;
  mat->Get(AI_MATKEY_NAME, mat_name);
  // Assimp material names can be empty, so we use a fallback name based on the model path and material index to ensure
  // uniqueness for caching in the assets manager
  std::string material_name = mat_name.length > 0 ? mat_name.C_Str() : "Material_" + std::to_string(index);
  std::string material_id = m_path + "::" + material_name;

  return assets_manager.get_or_load<Material>(material_id, m_shader, textures, params, state);
}

const std::map<std::string, std::string, NumericComparator>& Model::to_map() {
  if (not m_info.empty())
    return m_info;

  m_info["type"] = "Model";
  m_info["uuid"] = m_uuid;
  m_info["path"] = m_path;
  for (const auto& submesh : m_submeshes) {
    if (submesh.material.is_valid()) {
      m_info[submesh.material.get()->get_uuid().data()] = submesh.material.get()->get_uuid().data();
    } else {
      m_info["unvalid_material_" + std::to_string(&submesh - &m_submeshes[0])] = "undefined";
    }
  }

  return m_info;
}

const std::map<std::string, std::string, NumericComparator>& Model::get_material_info(std::string_view material_id) {
  auto it = std::find_if(m_submeshes.begin(), m_submeshes.end(), [&](const SubMesh& submesh) {
    return submesh.material.is_valid() && submesh.material.get()->get_uuid() == material_id;
  });
  if (it != m_submeshes.end()) {
    return it->material.get()->to_map();
  }
  return std::map<std::string, std::string, NumericComparator>{
    {"error", std::format("material with uuid {} not found in model", material_id)}};
}

bool Model::operator==(const Asset& other) const {
  ENGINE_ASSERT(typeid(other) == typeid(Model), "cannot compare model with other asset type!");
  return m_path == static_cast<const Model&>(other).m_path;
}

}