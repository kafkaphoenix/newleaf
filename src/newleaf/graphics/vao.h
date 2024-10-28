#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "../utils/numeric_comparator.h"
#include "buffer.h"

namespace nl {

class VAO {
  public:
    VAO();
    ~VAO();

    void bind();
    void unbind();

    enum class VertexType { vertex, shape_vertex, terrain_vertex };
    void attach_vertex(std::shared_ptr<VBO>&& vbo, VertexType type);
    void attach_vertex_attributes();
    void attach_shape_vertex_attributes();
    void attach_terrain_vertex_attributes();
    void update_vertex(std::shared_ptr<VBO>&& vbo, uint32_t index,
                       VertexType type);
    void clear_vbos();
    void set_index(std::unique_ptr<IBO>&& ibo);

    const std::vector<std::shared_ptr<VBO>>& get_vbos() const { return m_vbos; }
    const IBO& get_ebo() const { return *m_ibo; }
    uint32_t get_vbo_index() const { return m_vbo_index; }
    uint32_t get_id() const { return m_id; }
    bool is_binded() const { return m_binded; }
    const std::map<std::string, std::string, NumericComparator>& to_map();

    static std::shared_ptr<VAO> create();

  private:
    uint32_t m_id{};
    uint32_t m_vbo_index{};
    std::vector<std::shared_ptr<VBO>> m_vbos;
    std::unique_ptr<IBO> m_ibo;
    std::map<std::string, std::string, NumericComparator> m_info;
    bool m_dirty{};
    bool m_binded{};
};

}