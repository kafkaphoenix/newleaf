#include "buffer.h"

#include <glad/glad.h>

#include "../logging/log_manager.h"

namespace nl {

static constexpr GLbitfield mapping_flags =
  GL_MAP_WRITE_BIT | GL_MAP_READ_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;
static constexpr GLbitfield storage_flags =
  GL_DYNAMIC_STORAGE_BIT | mapping_flags; // allow modification of the buffer but not resizing

VBO::VBO(const std::vector<ModelVertex>& vertices) : m_count(vertices.size()) {
  glCreateBuffers(1, &m_id);
  glNamedBufferStorage(m_id, sizeof(ModelVertex) * vertices.size(), vertices.data(), storage_flags);
}

VBO::VBO(const std::vector<ShapeVertex>& vertices) : m_count(vertices.size()) {
  glCreateBuffers(1, &m_id);
  glNamedBufferStorage(m_id, sizeof(ShapeVertex) * vertices.size(), vertices.data(), storage_flags);
}

VBO::VBO(const std::vector<TerrainVertex>& vertices) : m_count(vertices.size()) {
  glCreateBuffers(1, &m_id);
  glNamedBufferStorage(m_id, sizeof(TerrainVertex) * vertices.size(), vertices.data(), storage_flags);
}

VBO::~VBO() {
  ENGINE_WARN("deleting vbo {}", m_id);
  glDeleteBuffers(1, &m_id);
}

IBO::IBO(const std::vector<uint32_t>& indices) : m_count(indices.size()) {
  glCreateBuffers(1, &m_id);
  glNamedBufferStorage(m_id, sizeof(uint32_t) * indices.size(), indices.data(), storage_flags);
}

void IBO::reload(const std::vector<uint32_t>& indices) {
  glNamedBufferSubData(m_id, 0, sizeof(uint32_t) * indices.size(), indices.data());
  m_count = indices.size();
}

IBO::~IBO() {
  ENGINE_WARN("deleting ibo {}", m_id);
  glDeleteBuffers(1, &m_id);
}

std::unique_ptr<IBO> IBO::create(const std::vector<uint32_t>& indices) { return std::make_unique<IBO>(indices); }
}