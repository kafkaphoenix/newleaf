#pragma once

#define GLM_FORCE_CTOR_INIT
#include <glm/glm.hpp>

#include "pch.h"

namespace potatoengine {
#define MAX_BONE_INFLUENCE 4 // TODO use

struct Vertex { // TODO move
    glm::vec3 position{};
    glm::vec3 normal{};
    glm::vec2 textureCoords{};
    glm::vec3 tangent{};
    glm::vec3 bitangent{};
    int boneIDs[MAX_BONE_INFLUENCE]{};
    float boneWeights[MAX_BONE_INFLUENCE]{};
    glm::vec4 color{glm::vec4(0.9725f, 0.f, 0.9725f, 1.f)};
};

struct ShapeVertex {
    glm::vec3 position{};
    glm::vec2 textureCoords{};
};

struct TerrainVertex {
    glm::vec3 position{};
    glm::vec3 normal{};
    glm::vec2 textureCoords{};
    glm::vec3 color{};
};

class VBO {
  public:
    VBO(const std::vector<Vertex>& vertices);
    VBO(const std::vector<ShapeVertex>& vertices);
    VBO(const std::vector<TerrainVertex>& vertices);
    ~VBO();

    void reload(const std::vector<Vertex>& vertices);

    uint32_t getCount() const { return m_count; }
    uint32_t getID() const { return m_id; }
    bool isImmutable() const { return m_immutable; }

    static std::unique_ptr<VBO> Create(const std::vector<Vertex>& vertices);
    static std::unique_ptr<VBO>
    CreateShape(const std::vector<ShapeVertex>& vertices);
    static std::unique_ptr<VBO>
    CreateTerrain(const std::vector<TerrainVertex>& vertices);

  private:
    uint32_t m_id{};
    uint32_t m_count{};
    bool m_immutable{};
};

class IBO {
  public:
    IBO(const std::vector<uint32_t>& indices);
    ~IBO();

    void reload(const std::vector<uint32_t>& indices);

    uint32_t getCount() const { return m_count; }
    uint32_t getID() const { return m_id; }
    bool isImmutable() const { return m_immutable; }

    static std::unique_ptr<IBO> Create(const std::vector<uint32_t>& indices);

  private:
    uint32_t m_id{};
    uint32_t m_count{};
    bool m_immutable{};
};
}