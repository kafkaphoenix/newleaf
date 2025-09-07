#pragma once

#include <memory>
#include <vector>

#define GLM_FORCE_CTOR_INIT
#include <glm/glm.hpp>

namespace nl {
#define MAX_BONE_INFLUENCE 4 // TODO use

struct ModelVertex { // TODO move?
    glm::vec3 position{};
    glm::vec3 normal{};
    glm::vec2 texture_coords{};
    glm::vec3 tangent{};
    glm::vec3 bitangent{};
    int bone_ids[MAX_BONE_INFLUENCE]{};
    float bone_weights[MAX_BONE_INFLUENCE]{};
};

struct ShapeVertex {
    glm::vec3 position{};
    glm::vec2 texture_coords{};
};

struct TerrainVertex {
    glm::vec3 position{};
    glm::vec3 normal{};
    glm::vec2 texture_coords{};
    glm::vec3 color{};
};

class VBO {
  public:
    VBO(const std::vector<ModelVertex>& vertices);
    VBO(const std::vector<ShapeVertex>& vertices);
    VBO(const std::vector<TerrainVertex>& vertices);
    ~VBO();

    uint32_t get_count() const { return m_count; }
    uint32_t get_id() const { return m_id; }

    static std::unique_ptr<VBO> CreateModel(const std::vector<ModelVertex>& vertices);
    static std::unique_ptr<VBO> CreateShape(const std::vector<ShapeVertex>& vertices);
    static std::unique_ptr<VBO> CreateTerrain(const std::vector<TerrainVertex>& vertices);

  private:
    uint32_t m_id{};
    uint32_t m_count{};
};

class IBO {
  public:
    IBO(const std::vector<uint32_t>& indices);
    ~IBO();

    void reload(const std::vector<uint32_t>& indices);

    uint32_t get_count() const { return m_count; }
    uint32_t get_id() const { return m_id; }

    static std::unique_ptr<IBO> create(const std::vector<uint32_t>& indices);

  private:
    uint32_t m_id{};
    uint32_t m_count{};
};
}