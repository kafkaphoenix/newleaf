#include "../assets/model.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#include "../core/application.h"
#include "../render/buffer.h"

namespace nl::assets {

Model::Model(std::filesystem::path&& fp, std::optional<bool> gammaCorrection)
  : m_path(std::move(fp.string())),
    m_directory(std::move(fp.parent_path().string())) {
  ENGINE_ASSERT(not gammaCorrection.has_value(),
                "Gamma correction not yet implemented");

  Assimp::Importer importer;
  const aiScene* scene = importer.ReadFile(
    m_path, aiProcess_Triangulate | aiProcess_GenSmoothNormals |
              aiProcess_CalcTangentSpace | aiProcess_ValidateDataStructure |
              aiProcess_JoinIdenticalVertices | aiProcess_OptimizeMeshes |
              aiProcess_OptimizeGraph | aiProcess_SplitLargeMeshes |
              aiProcess_FindInvalidData);

  ENGINE_ASSERT(scene and scene->mFlags not_eq AI_SCENE_FLAGS_INCOMPLETE and
                  scene->mRootNode,
                "Failed to load model {}: {}", m_path,
                importer.GetErrorString());

  process_node(scene->mRootNode, scene);
}

void Model::process_node(aiNode* node, const aiScene* scene) {
  m_meshes.reserve(node->mNumMeshes);
  for (uint32_t i = 0; i < node->mNumMeshes; ++i) {
    aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
    m_meshes.emplace_back(std::move(process_mesh(mesh, scene)));
  }

  for (uint32_t i = 0; i < node->mNumChildren; ++i) {
    process_node(node->mChildren[i], scene);
  }
}

CMesh Model::process_mesh(aiMesh* mesh, const aiScene* scene) {
  std::vector<Vertex> vertices{};
  std::vector<uint32_t> indices{};
  std::vector<std::shared_ptr<Texture>> textures;

  for (uint32_t i = 0; i < mesh->mNumVertices; ++i) {
    Vertex vertex{};
    const auto& position =
      mesh->mVertices[i]; // assimp vector does not directly convert to glm vec3
    vertex.position = glm::vec3(position.x, position.y, position.z);

    if (mesh->HasNormals()) {
      const auto& normalVector = mesh->mNormals[i];
      vertex.normal = glm::vec3(normalVector.x, normalVector.y, normalVector.z);
    }

    if (mesh->mTextureCoords[0]) {
      // a vertex can contain up to 8 different texture coordinates. We thus
      // make the assumption that we won't use models where a vertex can have
      // multiple texture coordinates so we always take the first set (0).
      const auto& texture_coords = mesh->mTextureCoords[0][i];
      vertex.texture_coords = glm::vec2(texture_coords.x, texture_coords.y);

      const auto& tangentVector = mesh->mTangents[i];
      vertex.tangent =
        glm::vec3(tangentVector.x, tangentVector.y, tangentVector.z);

      const auto& bitangentVector = mesh->mBitangents[i];
      vertex.bitangent =
        glm::vec3(bitangentVector.x, bitangentVector.y, bitangentVector.z);
    }

    if (mesh->HasBones()) {
      // TODO bones
    }

    if (mesh->HasVertexColors(0)) {
      const auto& color = mesh->mColors[0][i];
      vertex.color = glm::vec4(color.r, color.g, color.b, color.a);
    }

    if (mesh->mAABB.mMin not_eq aiVector3D(0.f, 0.f, 0.f) and
        mesh->mAABB.mMax not_eq aiVector3D(0.f, 0.f, 0.f)) {
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

  aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
  CMaterial materialData = load_material(material);

  // N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER.
  // diffuse: textureDiffuseN
  // specular: textureSpecularN
  // normal: textureNormalN
  // height: textureHeightN
  auto loadAndInsertTextures = [&](aiTextureType t, std::string type) {
    std::vector<std::shared_ptr<Texture>> loadedTextures =
      load_material_textures(material, t, type);
    textures.insert(textures.end(), loadedTextures.begin(),
                    loadedTextures.end()); // Can't be emplace
  };

  loadAndInsertTextures(aiTextureType_DIFFUSE, "textureDiffuse");
  loadAndInsertTextures(aiTextureType_SPECULAR, "textureSpecular");
  loadAndInsertTextures(aiTextureType_HEIGHT, "textureNormal");
  loadAndInsertTextures(aiTextureType_AMBIENT, "textureHeight");

  // 0.6 is the default value for diffuse in assimp
  if (textures.empty() and materialData.diffuse == glm::vec3(0.6f)) {
    const auto& assets_manager = Application::Get().get_assets_manager();
    if (assets_manager->contains<Texture>("default")) {
      textures.emplace_back(assets_manager->get<Texture>("default"));
    } else {
      textures.emplace_back(std::make_shared<Texture>(
        "assets/textures/default.jpg", "textureDiffuse"));
      assets_manager->load<assets::Texture>(
        "default", "assets/textures/default.jpg", "textureDiffuse");
    }
  }
  m_materials.emplace_back(std::move(materialData));

  return CMesh(std::move(vertices), std::move(indices), std::move(textures),
               std::string("camera"));
}

std::vector<std::shared_ptr<Texture>>
Model::load_material_textures(aiMaterial* mat, aiTextureType t,
                              std::string type) {
  std::vector<std::shared_ptr<Texture>> textures;
  textures.reserve(mat->GetTextureCount(t));
  for (uint32_t i = 0; i < mat->GetTextureCount(t); ++i) {
    aiString source;
    mat->GetTexture(t, i, &source);
    std::string filename = source.C_Str();
    std::string path = m_directory + "/" + filename;

    auto loadedTexture =
      std::find_if(m_loaded_textures.begin(), m_loaded_textures.end(),
                   [&](const std::shared_ptr<Texture>& texture) {
                     return texture->get_path() == path;
                   });

    if (loadedTexture not_eq m_loaded_textures.end()) {
      textures.emplace_back(std::make_shared<Texture>(*(*loadedTexture)));
    } else {
      std::shared_ptr<Texture> newTexture =
        std::make_shared<Texture>(path, type);
      textures.emplace_back(newTexture);
      m_loaded_textures.emplace_back(std::move(newTexture));
    }
  }

  return textures;
}

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

  m_info["Type"] = "Model";
  m_info["Path"] = m_path;
  m_info["Meshes"] = std::to_string(m_meshes.size());
  m_info["Materials"] = std::to_string(m_materials.size());
  for (uint32_t i = 0; i < m_loaded_textures.size(); ++i) {
    m_info["Loaded Texture " + std::to_string(i)] = std::to_string(i);
  }

  return m_info;
}

const std::map<std::string, std::string, NumericComparator>&
Model::get_loaded_texture_info(std::string_view textureID) {
  if (not m_loaded_texture_info.empty() and
      m_loaded_texture_info.contains(std::string(textureID))) {
    return m_loaded_texture_info.at(std::string(textureID));
  }

  m_loaded_texture_info[std::string(textureID)] =
    m_loaded_textures.at(std::stoi(textureID.data()))->to_map();

  return m_loaded_texture_info.at(std::string(textureID));
}

bool Model::operator==(const Asset& other) const {
  if (typeid(other) not_eq typeid(Model)) {
    ENGINE_ASSERT(false, "Cannot compare model with other asset type!");
  }
  const Model& otherModel = static_cast<const Model&>(other);
  return m_path == otherModel.m_path;
}

}
