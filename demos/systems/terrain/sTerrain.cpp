#include "systems/terrain/sTerrain.h"

#include <glm/glm.hpp>

using namespace entt::literals;

namespace demos::systems {

static constexpr glm::vec3 LIGHT_BLUE = {0.f, 0.9725f, 0.9725f};
static constexpr glm::vec3 LIGHT_YELLOW = {0.9725f, 0.9725f, 0.f};
static constexpr glm::vec3 LIGHT_GREEN = {0.f, 0.9725f, 0.f};
static constexpr glm::vec3 DARK_GREEN = {0.f, 0.5f, 0.f};
static constexpr glm::vec3 LIGHT_GREY = {0.5f, 0.5f, 0.5f};
static constexpr glm::vec3 WHITE = {0.9725f, 0.9725f, 0.9725f};

glm::vec3 calculateTriangleNormal(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3) {
  glm::vec3 u = v2 - v1;
  glm::vec3 v = v3 - v1;

  return glm::normalize(glm::cross(u, v));
}

glm::vec3 calculateQuadNormal(int col, int row,
                              const std::vector<std::vector<float>>& heights) {
  const float heightLeft = col > 0 ? heights[row][col - 1] : heights[row][col];
  const float heightRight =
    col < heights[row].size() - 1 ? heights[row][col + 1] : heights[row][col];
  const float heightDown = row > 0 ? heights[row - 1][col] : heights[row][col];
  const float heightUp =
    row < heights.size() - 1 ? heights[row + 1][col] : heights[row][col];

  return glm::normalize(
    glm::vec3(heightLeft - heightRight, 2.f, heightDown - heightUp));
}

glm::vec3 calculateBiomeColor(float height, uint32_t amplitude) {
  height = (height + amplitude) / (2 * amplitude);

  if (height < 0.2f) {
    return LIGHT_BLUE;
  } else if (height < 0.3f) {
    return LIGHT_YELLOW;
  } else if (height < 0.6f) {
    return LIGHT_GREEN;
  } else if (height < 0.8f) {
    return DARK_GREEN;
  } else if (height < 0.9f) {
    return LIGHT_GREY;
  } else {
    return WHITE;
  }
}

glm::vec3 calculateBiomeTexture(float height, uint32_t amplitude,
                                const engine::CTextureAtlas& cTextureAtlas) {
  height = (height + amplitude) / (2 * amplitude);

  int rows = cTextureAtlas.rows;
  int index = 5; // default

  if (height < 0.2f) { // TODO constants
    index = 4;         // water
  } else if (height < 0.3f) {
    index = 3; // sand
  } else if (height < 0.6f) {
    index = 2; // grass
  } else if (height < 0.8f) {
    index = 1; // dirt
  } else if (height < 0.9f) {
    index = 8; // stone
  } else {
    index = 6; // snow
  }

  uint32_t col = index % rows;
  float coll = static_cast<float>(col) / rows;
  uint32_t row = index / rows;
  float roww = static_cast<float>(row) / rows;

  return {static_cast<float>(coll) / rows, static_cast<float>(roww) / rows,
          0.f};
}

std::vector<std::vector<float>> generateHeights(uint32_t chunkSize,
                                                const engine::CNoise& noise,
                                                int xOffset, int zOffset) {
  std::vector<std::vector<float>> heights; // z = row, x = col
  heights.reserve(chunkSize + 1);
  for (int row = 0; row < chunkSize + 1; ++row) {
    heights.emplace_back(std::vector<float>());
    heights[row].reserve(chunkSize + 1);
    for (int col = 0; col < chunkSize + 1; ++col) {
      heights[row].emplace_back(noise.getNoise(
        static_cast<float>(col + xOffset), static_cast<float>(row + zOffset)));
    }
  }

  return heights;
}

std::vector<std::vector<glm::vec3>>
generateBiomes(std::vector<std::vector<float>> heights, uint32_t amplitude) {
  std::vector<std::vector<glm::vec3>> biomes; // z = row, x = col
  biomes.reserve(heights.size());
  for (uint32_t row = 0; row < heights.size(); ++row) {
    biomes.emplace_back(std::vector<glm::vec3>());
    biomes[row].reserve(heights[row].size());
    for (uint32_t col = 0; col < heights[row].size(); ++col) {
      biomes[row].emplace_back(
        calculateBiomeColor(heights[row][col], amplitude));
    }
  }

  return biomes;
}

std::vector<std::vector<glm::vec3>>
generateBiomesTextures(std::vector<std::vector<float>> heights,
                       uint32_t amplitude,
                       const engine::CTextureAtlas& cTextureAtlas) {
  std::vector<std::vector<glm::vec3>> textures; // z = row, x = col
  textures.reserve(heights.size());
  for (uint32_t row = 0; row < heights.size(); ++row) {
    textures.emplace_back(std::vector<glm::vec3>());
    textures[row].reserve(heights[row].size());
    for (uint32_t col = 0; col < heights[row].size(); ++col) {
      textures[row].emplace_back(
        calculateBiomeTexture(heights[row][col], amplitude, cTextureAtlas));
    }
  }

  return textures;
}

engine::CMesh generateTriangleMesh(
  uint32_t chunkSize, uint32_t blockSize,
  const std::vector<std::vector<float>>& heights,
  const std::optional<
    std::reference_wrapper<std::vector<std::vector<glm::vec3>>>>
    biomes = std::nullopt) {
  engine::CMesh mesh;
  uint32_t totalVertices =
    (chunkSize + 1) *
    (chunkSize + 1); // Triangles share vertices making impossible to use more
                     // than one texture for all the mesh

  std::vector<engine::TerrainVertex> vertices;
  vertices.reserve(totalVertices);

  std::vector<uint32_t> indices;
  indices.reserve(chunkSize * chunkSize * 6);

  // c++ is row major order (x, y, z) -> (width, height, depth) -> (col, height,
  // row)
  for (uint32_t row = 0; row < chunkSize + 1; ++row) {
    for (uint32_t col = 0; col < chunkSize + 1; ++col) {
      float x = static_cast<float>(col) * blockSize;
      float y = heights[row][col] * blockSize;
      float z = static_cast<float>(row) * blockSize;

      glm::vec3 position(x, y, z);
      glm::vec3 normal = std::move(calculateQuadNormal(
        col, row, heights)); // TODO maybe this normal is not correct and should
                             // call the other method
      glm::vec2 textureCoords{};
      glm::vec3 color{};
      if (biomes.has_value()) { // color per vertex
        color = biomes.value().get()[row][col];
      } else { // one texture for all the mesh
        textureCoords = {static_cast<float>(col) / chunkSize,
                         static_cast<float>(row) / chunkSize};
      }

      vertices.push_back({position, normal, textureCoords, color});
    }
  }

  for (uint32_t row = 0; row < chunkSize; ++row) {
    for (uint32_t col = 0; col < chunkSize; ++col) {
      uint32_t topLeft = (row * (chunkSize + 1)) + col;
      uint32_t topRight = topLeft + 1;
      uint32_t bottomLeft = ((row + 1) * (chunkSize + 1)) + col;
      uint32_t bottomRight = bottomLeft + 1;

      indices.push_back(topLeft);
      indices.push_back(bottomLeft);
      indices.push_back(topRight);
      indices.push_back(topRight);
      indices.push_back(bottomLeft);
      indices.push_back(bottomRight);
    }
  }

  mesh.vertexType = "terrain";
  mesh.vbo = engine::VBO::CreateTerrain(vertices);
  mesh.indices = std::move(indices);
  return mesh;
}

void addQuadVertexData(std::vector<engine::TerrainVertex>& vertices,
                       std::vector<uint32_t>& indices,
                       const std::array<glm::vec3, 4>& positions,
                       glm::vec3 normal,
                       const std::array<glm::vec2, 4>& textureCoordinates,
                       const std::array<glm::vec3, 4>& colors) {
  uint32_t topLeftIndex = vertices.size();
  uint32_t topRightIndex = topLeftIndex + 1;
  uint32_t bottomLeftIndex = topLeftIndex + 2;
  uint32_t bottomRightIndex = topLeftIndex + 3;

  vertices.push_back(
    {positions[0], normal, textureCoordinates[0], colors[0]}); // top left
  vertices.push_back(
    {positions[2], normal, textureCoordinates[2], colors[2]}); // top right
  vertices.push_back(
    {positions[1], normal, textureCoordinates[1], colors[1]}); // bottom left
  vertices.push_back(
    {positions[3], normal, textureCoordinates[3], colors[3]}); // bottom right

  indices.push_back(topLeftIndex);
  indices.push_back(bottomLeftIndex);
  indices.push_back(topRightIndex);
  indices.push_back(topRightIndex);
  indices.push_back(bottomLeftIndex);
  indices.push_back(bottomRightIndex);
}

void addTriangleVertexData(std::vector<engine::TerrainVertex>& vertices,
                           std::vector<uint32_t>& indices, glm::vec3 first,
                           glm::vec3 second, glm::vec3 third, glm::vec3 normal,
                           glm::vec2 firstTextureCoordinates,
                           glm::vec2 secondTextureCoordinates,
                           glm::vec2 thirdTextureCoordinates,
                           glm::vec3 firstColor, glm::vec3 secondColor,
                           glm::vec3 thirdColor) {
  uint32_t firstIndex = vertices.size();
  uint32_t secondIndex = firstIndex + 1;
  uint32_t thirdIndex = firstIndex + 2;

  vertices.push_back({first, normal, firstTextureCoordinates, firstColor});
  vertices.push_back({second, normal, secondTextureCoordinates, secondColor});
  vertices.push_back({third, normal, thirdTextureCoordinates, thirdColor});

  indices.push_back(firstIndex);
  indices.push_back(secondIndex);
  indices.push_back(thirdIndex);
}

std::array<glm::vec3, 4>
calculateQuadPositions(uint32_t col, uint32_t row,
                       const std::vector<std::vector<float>>& heights,
                       uint32_t blockSize) {
  std::array<glm::vec3, 4> positions;
  positions[0] = {static_cast<float>(col) * blockSize,
                  heights[row][col] * blockSize,
                  static_cast<float>(row) * blockSize}; // top left
  positions[1] = {static_cast<float>(col) * blockSize,
                  heights[row + 1][col] * blockSize,
                  static_cast<float>(row + 1) * blockSize}; // bottom left
  positions[2] = {static_cast<float>(col + 1) * blockSize,
                  heights[row][col + 1] * blockSize,
                  static_cast<float>(row) * blockSize}; // top right
  positions[3] = {static_cast<float>(col + 1) * blockSize,
                  heights[row + 1][col + 1] * blockSize,
                  static_cast<float>(row + 1) * blockSize}; // bottom right

  return positions;
}

std::array<glm::vec3, 4>
calculateBiomeColors(uint32_t col, uint32_t row,
                     const std::vector<std::vector<glm::vec3>>& biomes) {
  std::array<glm::vec3, 4> colors;
  colors[0] = biomes[row][col];         // top left
  colors[1] = biomes[row + 1][col];     // bottom left
  colors[2] = biomes[row][col + 1];     // top right
  colors[3] = biomes[row + 1][col + 1]; // bottom right

  return colors;
}

std::array<glm::vec2, 4>
calculateBiomeTextures(uint32_t col, uint32_t row,
                       const std::vector<std::vector<glm::vec3>>& biomes) {
  std::array<glm::vec2, 4>
    textureCoordinates; // TODO Do i need to multiply by blocksize?
  // TODO this is the offset I still need the texture coordinates
  textureCoordinates[0] = glm::vec2(biomes[row][col]);         // top left
  textureCoordinates[1] = glm::vec2(biomes[row + 1][col]);     // bottom left
  textureCoordinates[2] = glm::vec2(biomes[row][col + 1]);     // top right
  textureCoordinates[3] = glm::vec2(biomes[row + 1][col + 1]); // bottom right

  return textureCoordinates;
}

void addQuad(std::vector<engine::TerrainVertex>& vertices,
             std::vector<uint32_t>& indices, uint32_t col, uint32_t row,
             uint32_t chunkSize, uint32_t blockSize, bool duplicateVertices,
             engine::CTexture::DrawMode drawMode,
             const std::vector<std::vector<float>>& heights,
             const std::optional<
               std::reference_wrapper<std::vector<std::vector<glm::vec3>>>>
               biomes = std::nullopt) {
  std::array<glm::vec3, 4> positions =
    calculateQuadPositions(col, row, heights, blockSize);
  std::array<glm::vec2, 4> textureCoordinates{};
  std::array<glm::vec3, 4> colors{};

  if (biomes.has_value()) {
    if (drawMode == engine::CTexture::DrawMode::COLOR) {
      colors = calculateBiomeColors(col, row, biomes.value());
    } else if (drawMode == engine::CTexture::DrawMode::TEXTURE_ATLAS) {
      textureCoordinates = calculateBiomeTextures(col, row, biomes.value());
    }
  }

  if (duplicateVertices) {
    addTriangleVertexData(
      vertices, indices, positions[0], positions[1], positions[2],
      calculateTriangleNormal(positions[0], positions[1], positions[2]),
      textureCoordinates[0], textureCoordinates[1], textureCoordinates[2],
      colors[0], colors[1], colors[2]);
    addTriangleVertexData(
      vertices, indices, positions[2], positions[1], positions[3],
      calculateTriangleNormal(positions[2], positions[1], positions[3]),
      textureCoordinates[2], textureCoordinates[1], textureCoordinates[3],
      colors[2], colors[1], colors[3]);
  } else {
    addQuadVertexData(vertices, indices, positions,
                      calculateQuadNormal(col, row, heights),
                      textureCoordinates, colors);
  }
}

engine::CMesh
generateQuadMesh(uint32_t chunkSize, uint32_t blockSize, bool duplicateVertices,
                 engine::CTexture::DrawMode drawMode,
                 const std::vector<std::vector<float>>& heights,
                 const std::optional<
                   std::reference_wrapper<std::vector<std::vector<glm::vec3>>>>
                   biomes = std::nullopt) {
  engine::CMesh mesh;
  uint32_t totalVertices =
    chunkSize * chunkSize * 4; // 4 vertices per quad, triangles share vertices
  if (duplicateVertices) {
    totalVertices =
      chunkSize * chunkSize *
      6; // 6 vertices per quad (2 triangles) for higher quality lighting
  }

  std::vector<engine::TerrainVertex> vertices;
  vertices.reserve(totalVertices);

  std::vector<uint32_t> indices;
  indices.reserve(chunkSize * chunkSize * 6);

  // c++ is row major order (x, y, z) -> (width, height, depth) -> (col, height,
  // row)
  for (uint32_t row = 0; row < chunkSize; ++row) {
    for (uint32_t col = 0; col < chunkSize; ++col) {
      if (biomes.has_value()) {
        addQuad(vertices, indices, col, row, chunkSize, blockSize,
                duplicateVertices, drawMode, heights, biomes);
      } else {
        addQuad(vertices, indices, col, row, chunkSize, blockSize,
                duplicateVertices, drawMode, heights);
      }
    }
  }

  mesh.vertexType = "terrain";
  mesh.vbo = engine::VBO::CreateTerrain(vertices);
  mesh.indices = std::move(indices);
  return mesh;
}

engine::CMesh
generateTerrain(engine::CChunkManager::MeshType meshType,
                engine::CChunkManager::MeshAlgorithm meshAlgorithm,
                uint32_t chunkSize, uint32_t blockSize,
                engine::CTexture::DrawMode drawMode,
                const std::vector<std::vector<float>>& heights,
                const std::optional<
                  std::reference_wrapper<std::vector<std::vector<glm::vec3>>>>
                  biomes = std::nullopt) {
  if (meshType == engine::CChunkManager::MeshType::Plane) {
    if (meshAlgorithm ==
        engine::CChunkManager::MeshAlgorithm::SimplifiedTriangle) {
      if (biomes.has_value()) {
        return generateTriangleMesh(chunkSize, blockSize, heights, biomes);
      } else if (drawMode == engine::CTexture::DrawMode::TEXTURE) {
        return generateTriangleMesh(chunkSize, blockSize, heights);
      } else { // TODO improve log to show string and not int
        ENGINE_ASSERT(
          false,
          "Draw mode {} not supported for mesh type {} and mesh algorithm {}",
          static_cast<int>(drawMode), static_cast<int>(meshType),
          static_cast<int>(meshAlgorithm));
      }
    } else if (meshAlgorithm ==
                 engine::CChunkManager::MeshAlgorithm::SimplifiedQuad or
               meshAlgorithm == engine::CChunkManager::MeshAlgorithm::Quad) {
      bool duplicateVertices =
        meshAlgorithm == engine::CChunkManager::MeshAlgorithm::Quad;
      if (biomes.has_value()) {
        return generateQuadMesh(chunkSize, blockSize, duplicateVertices,
                                drawMode, heights, biomes);
      } else {
        ENGINE_ASSERT(
          false,
          "Draw mode {} not supported for mesh type {} and mesh algorithm {}",
          static_cast<int>(drawMode), static_cast<int>(meshType),
          static_cast<int>(meshAlgorithm));
      }
    }
  } else if (meshType == engine::CChunkManager::MeshType::Chunk) {
    if (meshAlgorithm == engine::CChunkManager::MeshAlgorithm::Greedy) {
      // return generateChunkMesh(chunkSize, blockSize, heights);
    }
  } else if (meshType == engine::CChunkManager::MeshType::Sphere) {
    // return generateSphereMesh(chunkSize, blockSize, heights);
  } else {
    ENGINE_ERROR("Mesh type {} not supported", static_cast<int>(meshType));
  }
  return engine::CMesh{};
}

void TerrainSystem::init(entt::registry& registry) {
  registry
    .view<engine::CChunkManager, engine::CTexture, engine::CNoise,
          engine::CUUID>()
    .each([&](entt::entity e, engine::CChunkManager& cChunkManager,
              const engine::CTexture& cTexture, const engine::CNoise& cNoise,
              const engine::CUUID& cUUID) {
      engine::CTextureAtlas* cTextureAtlas = nullptr;
      if (cTexture.drawMode == engine::CTexture::DrawMode::TEXTURE_ATLAS or
          cTexture.drawMode ==
            engine::CTexture::DrawMode::TEXTURE_ATLAS_BLEND or
          cTexture.drawMode ==
            engine::CTexture::DrawMode::TEXTURE_ATLAS_BLEND_COLOR) {
        cTextureAtlas = registry.try_get<engine::CTextureAtlas>(e);
        ENGINE_ASSERT(cTextureAtlas, "Texture atlas not found for entity: {}",
                      cUUID.uuid);
      }

      // Create chunks around 0 0 0
      // TODO y axis should infinite, maybe rename z to depth
      // TODO fix this, it's not working heights is always the same maybe is
      // something with int and uint32_t?
      std::vector<std::vector<float>> heights;
      std::vector<std::vector<glm::vec3>> biomes;
      engine::CChunk chunk;
      for (int row = -cChunkManager.width; row <= cChunkManager.width; ++row) {
        for (int col = -cChunkManager.height; col <= cChunkManager.height;
             ++col) {
          chunk = engine::CChunk{"plains"};
          heights = generateHeights(cChunkManager.chunkSize, cNoise, col, row);
          if (cChunkManager.useBiomes) {
            if (cTexture.drawMode == engine::CTexture::DrawMode::COLOR) {
              biomes = generateBiomes(heights, cNoise.amplitude);
            } else if (cTexture.drawMode ==
                         engine::CTexture::DrawMode::TEXTURE_ATLAS or
                       cTexture.drawMode ==
                         engine::CTexture::DrawMode::TEXTURE_ATLAS_BLEND or
                       cTexture.drawMode == engine::CTexture::DrawMode::
                                              TEXTURE_ATLAS_BLEND_COLOR) {
              biomes = generateBiomesTextures(heights, cNoise.amplitude,
                                              *cTextureAtlas);
            }
            chunk.terrainMesh = std::move(generateTerrain(
              cChunkManager.meshType, cChunkManager.meshAlgorithm,
              cChunkManager.chunkSize, cChunkManager.blockSize,
              cTexture.drawMode, heights, biomes));
          } else {
            chunk.terrainMesh = std::move(generateTerrain(
              cChunkManager.meshType, cChunkManager.meshAlgorithm,
              cChunkManager.chunkSize, cChunkManager.blockSize,
              cTexture.drawMode, heights));
          }
          chunk.transform.position =
            glm::vec3{static_cast<float>(col) * cChunkManager.chunkSize *
                        cChunkManager.blockSize,
                      0.f,
                      static_cast<float>(row) * cChunkManager.chunkSize *
                        cChunkManager.blockSize};
          cChunkManager.chunks.emplace(
            glm::vec3{static_cast<float>(col) * cChunkManager.chunkSize *
                        cChunkManager.blockSize,
                      0.f,
                      static_cast<float>(row) * cChunkManager.chunkSize *
                        cChunkManager.blockSize},
            std::move(chunk)); // TODO check if this work with blocksize
        }
      }
    });
}

void TerrainSystem::update(entt::registry& registry, const engine::Time& ts) {
  auto& app = engine::Application::Get();
  if (app.isGamePaused()) {
    return;
  }

  // TODO here we rebuild meshes and stuff
}
}