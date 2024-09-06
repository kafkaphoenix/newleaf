#include "../render/shader_program.h"

#include <glm/gtc/type_ptr.hpp>

#include "../core/log_manager.h"

namespace nl {
ShaderProgram::ShaderProgram(std::string&& name)
  : m_id(glCreateProgram()), m_name(std::move(name)) {}

ShaderProgram::~ShaderProgram() {
  ENGINE_WARN("Deleting shader program {}", m_name);
  glDeleteProgram(m_id);
}

ShaderProgram::operator GLuint() const { return static_cast<GLuint>(m_id); }

void ShaderProgram::attach(const assets::Shader& s) { glAttachShader(m_id, s); }

void ShaderProgram::detach(const assets::Shader& s) { glDetachShader(m_id, s); }

void ShaderProgram::link() {
  glLinkProgram(m_id);

  int status = GL_FALSE;
  glGetProgramiv(m_id, GL_LINK_STATUS, &status);
  if (status not_eq GL_TRUE) [[unlikely]] {
    int infoLogLength = 0;
    glGetProgramiv(m_id, GL_INFO_LOG_LENGTH, &infoLogLength);
    ENGINE_ASSERT(infoLogLength > 0, "Shader program {} linking failed!",
                  m_name);
    std::vector<GLchar> shaderProgramInfoLog(infoLogLength);
    glGetProgramInfoLog(m_id, infoLogLength, &infoLogLength,
                        shaderProgramInfoLog.data());
    ENGINE_ASSERT(false, "Shader program {} linking failed: \n{}", m_name,
                  std::string(shaderProgramInfoLog.data()));
  }
  m_active_uniforms = get_active_uniforms();
  print_active_uniforms();
}

void ShaderProgram::use() { glUseProgram(m_id); }

void ShaderProgram::unuse() { glUseProgram(0); }

void ShaderProgram::set_int(std::string_view name, int value) {
  glUniform1i(glGetUniformLocation(m_id, name.data()), value);
}

void ShaderProgram::set_float(std::string_view name, float value) {
  glUniform1f(glGetUniformLocation(m_id, name.data()), value);
}

void ShaderProgram::set_vec2(std::string_view name, const glm::vec2& v) {
  glUniform2f(glGetUniformLocation(m_id, name.data()), v.x, v.y);
}

void ShaderProgram::set_vec3(std::string_view name, const glm::vec3& v) {
  glUniform3f(glGetUniformLocation(m_id, name.data()), v.x, v.y, v.z);
}

void ShaderProgram::set_vec4(std::string_view name, const glm::vec4& v) {
  glUniform4f(glGetUniformLocation(m_id, name.data()), v.x, v.y, v.z, v.w);
}

void ShaderProgram::set_mat4(std::string_view name, const glm::mat4& m) {
  glUniformMatrix4fv(glGetUniformLocation(m_id, name.data()), 1, GL_FALSE,
                     glm::value_ptr(m));
}

std::vector<ActiveUniform> ShaderProgram::get_active_uniforms() {
  GLint numActiveUniforms = 0;
  glGetProgramInterfaceiv(m_id, GL_UNIFORM, GL_ACTIVE_RESOURCES,
                          &numActiveUniforms);

  std::vector<ActiveUniform> activeUniforms;

  std::vector<GLenum> properties;
  properties.reserve(3);
  properties.emplace_back(GL_NAME_LENGTH);
  properties.emplace_back(GL_TYPE);
  properties.emplace_back(GL_ARRAY_SIZE);
  std::vector<GLint> values(properties.size());

  std::vector<GLchar> nameData(256);

  activeUniforms.reserve(numActiveUniforms);
  for (int i = 0; i < numActiveUniforms; ++i) {
    glGetProgramResourceiv(m_id, GL_UNIFORM, i, properties.size(),
                           &properties[0], values.size(), nullptr, &values[0]);

    nameData.resize(values[0]);
    glGetProgramResourceName(m_id, GL_UNIFORM, i, nameData.size(), nullptr,
                             &nameData[0]);
    std::string name(reinterpret_cast<char*>(&nameData[0]), values[0] - 1);

    ActiveUniform uniform;
    uniform.type = values[1];
    uniform.name = name;

    activeUniforms.emplace_back(std::move(uniform));
  }

  return activeUniforms;
}

void ShaderProgram::reset_active_uniforms() {
  use();
  for (const auto& [type, name] : m_active_uniforms) {
    if (type == GL_INT) {
      set_int(name, 0);
    } else if (type == GL_FLOAT) {
      set_float(name, 0.f);
    } else if (type == GL_FLOAT_VEC2) {
      set_vec2(name, glm::vec2(0.f));
    } else if (type == GL_FLOAT_VEC3) {
      set_vec3(name, glm::vec3(0.f));
    } else if (type == GL_FLOAT_VEC4) {
      set_vec4(name, glm::vec4(0.f));
    } else if (type == GL_FLOAT_MAT4) {
      set_mat4(name, glm::mat4(1.f));
    } else if (type == GL_SAMPLER_2D) {
      set_int(name, 0);
    } else if (type == GL_SAMPLER_CUBE) {
      set_int(name, 0);
    } else {
      ENGINE_ASSERT(false, "Unknown uniform type {} for uniform {}", type,
                    name);
    }
  }
  unuse();
}

void ShaderProgram::print_active_uniforms() {
  ENGINE_BACKTRACE(
    "===================Shader program {} Uniforms===================", m_name);
  for (const auto& [type, name] : m_active_uniforms) {
    if (type == GL_INT) {
      ENGINE_BACKTRACE("Uniform {} type: {}", name, "int");
    } else if (type == GL_FLOAT) {
      ENGINE_BACKTRACE("Uniform {} type: {}", name, "float");
    } else if (type == GL_FLOAT_VEC2) {
      ENGINE_BACKTRACE("Uniform {} type: {}", name, "vec2");
    } else if (type == GL_FLOAT_VEC3) {
      ENGINE_BACKTRACE("Uniform {} type: {}", name, "vec3");
    } else if (type == GL_FLOAT_VEC4) {
      ENGINE_BACKTRACE("Uniform {} type: {}", name, "vec4");
    } else if (type == GL_FLOAT_MAT4) {
      ENGINE_BACKTRACE("Uniform {} type: {}", name, "mat4");
    } else if (type == GL_SAMPLER_2D) {
      ENGINE_BACKTRACE("Uniform {} type: {}", name, "sampler2D");
    } else if (type == GL_SAMPLER_CUBE) {
      ENGINE_BACKTRACE("Uniform {} type: {}", name, "samplerCube");
    } else {
      ENGINE_ASSERT(false, "Unknown uniform type {} for uniform {}", type,
                    name);
    }
  }
  ENGINE_BACKTRACE("=====================================================");
}

const std::map<std::string, std::string, NumericComparator>&
ShaderProgram::to_map() {
  if (not m_info.empty()) {
    return m_info;
  }

  m_info["Name"] = m_name;
  m_info["id"] = std::to_string(m_id);
  for (const auto& [type, name] : m_active_uniforms) {
    if (type == GL_INT) {
      m_info["Uniform " + name] = "int";
    } else if (type == GL_FLOAT) {
      m_info["Uniform " + name] = "float";
    } else if (type == GL_FLOAT_VEC2) {
      m_info["Uniform " + name] = "vec2";
    } else if (type == GL_FLOAT_VEC3) {
      m_info["Uniform " + name] = "vec3";
    } else if (type == GL_FLOAT_VEC4) {
      m_info["Uniform " + name] = "vec4";
    } else if (type == GL_FLOAT_MAT4) {
      m_info["Uniform " + name] = "mat4";
    } else if (type == GL_SAMPLER_2D) {
      m_info["Uniform " + name] = "sampler2D";
    } else if (type == GL_SAMPLER_CUBE) {
      m_info["Uniform " + name] = "samplerCube";
    } else {
      ENGINE_ASSERT(false, "Unknown uniform type {} for uniform {}", type,
                    name);
    }
  }

  return m_info;
}

std::unique_ptr<ShaderProgram> ShaderProgram::Create(std::string&& name) {
  return std::make_unique<ShaderProgram>(std::move(name));
}

}