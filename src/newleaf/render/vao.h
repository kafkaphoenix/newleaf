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
    enum class VertexType { Model, Shape, Terrain };

    VAO();
    ~VAO();

    void bind() const;
    void unbind() const;

    void attach_vertex(std::unique_ptr<VBO>&& vbo, VertexType type);
    void set_index(std::unique_ptr<IBO>&& ibo);

    uint32_t get_id() const { return m_id; }
    std::string_view get_vertex_type() const;
    const VBO& get_vbo() const { return *m_vbo; }
    const IBO& get_ibo() const { return *m_ibo; }
    bool is_binded() const { return m_binded; }
    const std::map<std::string, std::string, NumericComparator>& to_map();

    static std::unique_ptr<VAO> create();

  private:
    void attach_model_vertex_attributes();
    void attach_shape_vertex_attributes();
    void attach_terrain_vertex_attributes();

    uint32_t m_id{};
    std::unique_ptr<VBO> m_vbo;
    std::unique_ptr<IBO> m_ibo;
    std::map<std::string, std::string, NumericComparator> m_info;
    VertexType m_vertex_type{VertexType::Model};
    bool m_dirty{};
    mutable bool m_binded{};
};

}