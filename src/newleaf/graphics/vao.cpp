#include "vao.h"

#include <glad/glad.h>

#include "../logging/log_manager.h"

namespace nl {

VAO::VAO() { glCreateVertexArrays(1, &m_id); }

VAO::~VAO() {
  ENGINE_WARN("deleting vao {}", m_id);
  glDeleteVertexArrays(1, &m_id);
}

void VAO::bind() const {
  glBindVertexArray(m_id);
  m_binded = true;
}

void VAO::unbind() const {
  glBindVertexArray(0);
  m_binded = false;
}

void VAO::attach_vertex(std::unique_ptr<VBO>&& vbo, VertexType type) {
  size_t stride = 0;
  m_vertex_type = type;
  if (type == VertexType::Model) {
    stride = sizeof(ModelVertex);
  } else if (type == VertexType::Shape) {
    stride = sizeof(ShapeVertex);
  } else if (type == VertexType::Terrain) {
    stride = sizeof(TerrainVertex);
  }

  glVertexArrayVertexBuffer(m_id, 0, vbo->get_id(), 0, stride);
  m_vbo = std::move(vbo);

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
  glVertexArrayAttribBinding(m_id, 0, 0);

  glEnableVertexArrayAttrib(m_id, 1);
  glVertexArrayAttribFormat(m_id, 1, 3, GL_FLOAT, GL_FALSE, offsetof(ModelVertex, normal));
  glVertexArrayAttribBinding(m_id, 1, 0);

  glEnableVertexArrayAttrib(m_id, 2);
  glVertexArrayAttribFormat(m_id, 2, 2, GL_FLOAT, GL_FALSE, offsetof(ModelVertex, texture_coords));
  glVertexArrayAttribBinding(m_id, 2, 0);

  glEnableVertexArrayAttrib(m_id, 3);
  glVertexArrayAttribFormat(m_id, 3, 3, GL_FLOAT, GL_FALSE, offsetof(ModelVertex, tangent));
  glVertexArrayAttribBinding(m_id, 3, 0);

  glEnableVertexArrayAttrib(m_id, 4);
  glVertexArrayAttribFormat(m_id, 4, 3, GL_FLOAT, GL_FALSE, offsetof(ModelVertex, bitangent));
  glVertexArrayAttribBinding(m_id, 4, 0);

  glEnableVertexArrayAttrib(m_id, 5);
  glVertexArrayAttribFormat(m_id, 5, 4, GL_INT, GL_FALSE, offsetof(ModelVertex, bone_ids));
  glVertexArrayAttribBinding(m_id, 5, 0);

  glEnableVertexArrayAttrib(m_id, 6);
  glVertexArrayAttribFormat(m_id, 6, 4, GL_FLOAT, GL_FALSE, offsetof(ModelVertex, bone_weights));
  glVertexArrayAttribBinding(m_id, 6, 0);
}

void VAO::attach_shape_vertex_attributes() {
  glEnableVertexArrayAttrib(m_id, 0);
  glVertexArrayAttribFormat(m_id, 0, 3, GL_FLOAT, GL_FALSE, offsetof(ShapeVertex, position));
  glVertexArrayAttribBinding(m_id, 0, 0);

  glEnableVertexArrayAttrib(m_id, 1);
  glVertexArrayAttribFormat(m_id, 1, 2, GL_FLOAT, GL_FALSE, offsetof(ShapeVertex, texture_coords));
  glVertexArrayAttribBinding(m_id, 1, 0);
}

void VAO::attach_terrain_vertex_attributes() {
  glEnableVertexArrayAttrib(m_id, 0);
  glVertexArrayAttribFormat(m_id, 0, 3, GL_FLOAT, GL_FALSE, offsetof(TerrainVertex, position));
  glVertexArrayAttribBinding(m_id, 0, 0);

  glEnableVertexArrayAttrib(m_id, 1);
  glVertexArrayAttribFormat(m_id, 1, 3, GL_FLOAT, GL_FALSE, offsetof(TerrainVertex, normal));
  glVertexArrayAttribBinding(m_id, 1, 0);

  glEnableVertexArrayAttrib(m_id, 2);
  glVertexArrayAttribFormat(m_id, 2, 2, GL_FLOAT, GL_FALSE, offsetof(TerrainVertex, texture_coords));
  glVertexArrayAttribBinding(m_id, 2, 0);

  glEnableVertexArrayAttrib(m_id, 3);
  glVertexArrayAttribFormat(m_id, 3, 4, GL_FLOAT, GL_FALSE, offsetof(TerrainVertex, color));
  glVertexArrayAttribBinding(m_id, 3, 0);
}

void VAO::set_index(std::unique_ptr<IBO>&& ibo) { // TODO: should not be binded here but renderer
  glVertexArrayElementBuffer(m_id, ibo->get_id());
  m_ibo = std::move(ibo);
  m_dirty = true;
}

std::string_view VAO::get_vertex_type() const {
  if (m_vertex_type == VertexType::Model) {
    return "model";
  } else if (m_vertex_type == VertexType::Shape) {
    return "shape";
  } else if (m_vertex_type == VertexType::Terrain) {
    return "terrain";
  }
  return "undefined";
}

const std::map<std::string, std::string, NumericComparator>& VAO::to_map() {
  if (not m_dirty) {
    return m_info;
  }

  m_info.clear();
  m_info["id"] = std::to_string(m_id);
  m_info["vbo_id"] = m_vbo ? std::to_string(m_vbo->get_id()) : "undefined";
  m_info["ibo_id"] = m_ibo ? std::to_string(m_ibo->get_id()) : "undefined";
  if (m_vertex_type == VertexType::Model) {
    m_info["vertex_type"] = "model";
  } else if (m_vertex_type == VertexType::Shape) {
    m_info["vertex_type"] = "shape";
  } else if (m_vertex_type == VertexType::Terrain) {
    m_info["vertex_type"] = "terrain";
  }
  m_dirty = false;

  return m_info;
}

std::unique_ptr<VAO> VAO::create() { return std::make_unique<VAO>(); }
}