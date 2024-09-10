#include "shader.h"

#include <fstream>

#include "../logging/log_manager.h"
#include "../utils/assert.h"

namespace nl::assets {
Shader::Shader(std::filesystem::path&& fp) : m_path(std::move(fp.string())) {
  std::ifstream f(fp);
  ENGINE_ASSERT(f.is_open(), "failed to open shader file!");
  ENGINE_ASSERT(f.peek() not_eq std::ifstream::traits_type::eof(),
                "shader file is empty!");
  std::string data(std::istreambuf_iterator<char>(f), {});
  f.close();

  m_type = fp.extension() == ".vert" ? GL_VERTEX_SHADER : GL_FRAGMENT_SHADER;
  m_id = glCreateShader(m_type);

  const GLchar* source = data.data();
  glShaderSource(m_id, 1, &source, 0);
  glCompileShader(m_id);

  int32_t status = GL_FALSE;
  glGetShaderiv(m_id, GL_COMPILE_STATUS, &status);
  if (status not_eq GL_TRUE) [[unlikely]] {
    int infoLogLength = 0;
    glGetShaderiv(m_id, GL_INFO_LOG_LENGTH, &infoLogLength);
    ENGINE_ASSERT(infoLogLength > 0, "shader {} compilation failed!", m_path);
    std::vector<GLchar> shaderInfoLog(infoLogLength);
    glGetShaderInfoLog(m_id, infoLogLength, &infoLogLength,
                       shaderInfoLog.data());
    glDeleteShader(m_id);
    ENGINE_ASSERT(false, "shader {} compilation failed: \n{}", m_path,
                  std::string(shaderInfoLog.data()));
  }
}

Shader::~Shader() {
  ENGINE_WARN("deleting shader {}", m_path);
  glDeleteShader(m_id);
}

const std::map<std::string, std::string, NumericComparator>& Shader::to_map() {
  if (not m_info.empty()) {
    return m_info;
  }

  m_info["type"] = "shader";
  m_info["path"] = m_path;
  m_info["id"] = std::to_string(m_id);
  if (m_type == GL_VERTEX_SHADER) {
    m_info["shader_type"] = "vertex";
  } else if (m_type == GL_FRAGMENT_SHADER) {
    m_info["shader_type"] = "fragment";
  } else if (m_type == GL_GEOMETRY_SHADER) {
    m_info["shader_type"] = "geometry";
  } else if (m_type == GL_TESS_CONTROL_SHADER) {
    m_info["shader_type"] = "tessellation control";
  } else if (m_type == GL_TESS_EVALUATION_SHADER) {
    m_info["shader_type"] = "tessellation evaluation";
  } else if (m_type == GL_COMPUTE_SHADER) {
    m_info["shader_type"] = "compute";
  } else {
    m_info["shader_type"] = "unknown";
  }

  return m_info;
}

bool Shader::operator==(const Asset& other) const {
  if (typeid(*this) not_eq typeid(other)) {
    ENGINE_ASSERT(false, "cannot compare shader with other asset type!");
  }
  const Shader& otherShader = static_cast<const Shader&>(other);
  return m_path == otherShader.m_path and m_type == otherShader.m_type and
         m_id == otherShader.m_id;
}
}