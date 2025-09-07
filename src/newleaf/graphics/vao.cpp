#include "vao.h"

#include <glad/glad.h>

#include "../logging/log_manager.h"

namespace nl {

VAO::VAO() { glCreateVertexArrays(1, &m_id); }

VAO::~VAO() {
  ENGINE_WARN("deleting vao {}", m_id);
  glDeleteVertexArrays(1, &m_id);
}

void VAO::bind() {
  glBindVertexArray(m_id);
  m_binded = true;
}

void VAO::unbind() {
  glBindVertexArray(0);
  m_binded = false;
}

void VAO::attach_vertex(std::shared_ptr<VBO>&& vbo, VertexType type) {
  size_t stride = 0;
  if (type == VertexType::Model) {
    stride = sizeof(ModelVertex);
  } else if (type == VertexType::Shape) {
    stride = sizeof(ShapeVertex);
  } else if (type == VertexType::Terrain) {
    stride = sizeof(TerrainVertex);
  }

  glVertexArrayVertexBuffer(m_id, 0, vbo->get_id(), 0, stride);
  m_vbos.emplace_back(std::move(vbo));

  if (type == VertexType::Model) {
    attach_model_vertex_attributes();
  } else if (type == VertexType::Shape) {
    attach_shape_vertex_attributes();
  } else if (type == VertexType::Terrain) {
    attach_terrain_vertex_attributes();
  }
  m_dirty = true;
}

void VAO::attach_model_vertex_attributes() {
  glEnableVertexArrayAttrib(m_id, 0);
  glVertexArrayAttribFormat(m_id, 0, 3, GL_FLOAT, GL_FALSE, offsetof(ModelVertex, position));
  glVertexArrayAttribBinding(m_id, 0, m_vbo_index);

  glEnableVertexArrayAttrib(m_id, 1);
  glVertexArrayAttribFormat(m_id, 1, 3, GL_FLOAT, GL_FALSE, offsetof(ModelVertex, normal));
  glVertexArrayAttribBinding(m_id, 1, m_vbo_index);

  glEnableVertexArrayAttrib(m_id, 2);
  glVertexArrayAttribFormat(m_id, 2, 2, GL_FLOAT, GL_FALSE, offsetof(ModelVertex, texture_coords));
  glVertexArrayAttribBinding(m_id, 2, m_vbo_index);

  glEnableVertexArrayAttrib(m_id, 3);
  glVertexArrayAttribFormat(m_id, 3, 3, GL_FLOAT, GL_FALSE, offsetof(ModelVertex, tangent));
  glVertexArrayAttribBinding(m_id, 3, m_vbo_index);

  glEnableVertexArrayAttrib(m_id, 4);
  glVertexArrayAttribFormat(m_id, 4, 3, GL_FLOAT, GL_FALSE, offsetof(ModelVertex, bitangent));
  glVertexArrayAttribBinding(m_id, 4, m_vbo_index);

  glEnableVertexArrayAttrib(m_id, 5);
  glVertexArrayAttribFormat(m_id, 5, 4, GL_INT, GL_FALSE, offsetof(ModelVertex, bone_ids));
  glVertexArrayAttribBinding(m_id, 5, m_vbo_index);

  glEnableVertexArrayAttrib(m_id, 6);
  glVertexArrayAttribFormat(m_id, 6, 4, GL_FLOAT, GL_FALSE, offsetof(ModelVertex, bone_weights));
  glVertexArrayAttribBinding(m_id, 6, m_vbo_index);

  ++m_vbo_index;
}

void VAO::attach_shape_vertex_attributes() {
  glEnableVertexArrayAttrib(m_id, 0);
  glVertexArrayAttribFormat(m_id, 0, 3, GL_FLOAT, GL_FALSE, offsetof(ShapeVertex, position));
  glVertexArrayAttribBinding(m_id, 0, m_vbo_index);

  glEnableVertexArrayAttrib(m_id, 1);
  glVertexArrayAttribFormat(m_id, 1, 2, GL_FLOAT, GL_FALSE, offsetof(ShapeVertex, texture_coords));
  glVertexArrayAttribBinding(m_id, 1, m_vbo_index);

  ++m_vbo_index;
}

void VAO::attach_terrain_vertex_attributes() {
  glEnableVertexArrayAttrib(m_id, 0);
  glVertexArrayAttribFormat(m_id, 0, 3, GL_FLOAT, GL_FALSE, offsetof(TerrainVertex, position));
  glVertexArrayAttribBinding(m_id, 0, m_vbo_index);

  glEnableVertexArrayAttrib(m_id, 1);
  glVertexArrayAttribFormat(m_id, 1, 3, GL_FLOAT, GL_FALSE, offsetof(TerrainVertex, normal));
  glVertexArrayAttribBinding(m_id, 1, m_vbo_index);

  glEnableVertexArrayAttrib(m_id, 2);
  glVertexArrayAttribFormat(m_id, 2, 2, GL_FLOAT, GL_FALSE, offsetof(TerrainVertex, texture_coords));
  glVertexArrayAttribBinding(m_id, 2, m_vbo_index);

  glEnableVertexArrayAttrib(m_id, 3);
  glVertexArrayAttribFormat(m_id, 3, 4, GL_FLOAT, GL_FALSE, offsetof(TerrainVertex, color));
  glVertexArrayAttribBinding(m_id, 3, m_vbo_index);

  ++m_vbo_index;
}

void VAO::clear_vbos() { // TODO: move to on detach on component? Do i need it?
                         // should not be binded here
  m_vbos.clear();
  m_vbo_index = 0;
  m_dirty = true;
}

void VAO::set_index(std::unique_ptr<IBO>&& ibo) { // TODO: should not be binded here
  glVertexArrayElementBuffer(m_id, ibo->get_id());
  m_ibo = std::move(ibo);
  m_dirty = true;
}

const std::map<std::string, std::string, NumericComparator>& VAO::to_map() {
  if (not m_dirty) {
    return m_info;
  }

  m_info.clear();
  m_info["id"] = std::to_string(m_id);
  m_info["vbo_index"] = std::to_string(m_vbo_index);
  for (uint32_t i = 0; i < m_vbos.size(); ++i) {
    m_info["vbo_" + std::to_string(i) + " id"] = std::to_string(m_vbos[i]->get_id());
  }
  m_info["ibo_id"] = std::to_string(m_ibo->get_id());
  m_dirty = false;

  return m_info;
}

std::shared_ptr<VAO> VAO::create() { return std::make_shared<VAO>(); }
}