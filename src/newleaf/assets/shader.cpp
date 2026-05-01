#include "shader.h"

#include <fstream>

#include <glm/gtc/type_ptr.hpp>

#include "../logging/log_manager.h"
#include "../utils/assert.h"

namespace nl {

namespace {

std::string load_file(const std::filesystem::path& path) {
  // open file in binary mode and move the file pointer to the end of the file to get the file size
  std::ifstream file(path, std::ios::binary | std::ios::ate);
  ENGINE_ASSERT(file.is_open(), "failed to open shader file: {}", path.string());

  // get file size
  const std::streamsize size = file.tellg();
  // reset file pointer to the beginning of the file
  file.seekg(0);

  // read file contents into a string with exactly the size of the file
  std::string buffer(size, '\0');
  file.read(buffer.data(), size);
  return buffer;
}

void check_shader_compilation(uint32_t shader_id, std::string_view name) {
  GLint success = GL_FALSE;
  glGetShaderiv(shader_id, GL_COMPILE_STATUS, &success);

  if (success != GL_TRUE) {
    GLint log_length = 0;
    glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &log_length);

    std::vector<char> log(log_length);
    glGetShaderInfoLog(shader_id, log_length, nullptr, log.data());

    ENGINE_ASSERT(false, "shader '{}' compilation failed: {}", name, log.data());
  }
}

void check_program_linking(uint32_t program_id) {
  GLint success = GL_FALSE;
  glGetProgramiv(program_id, GL_LINK_STATUS, &success);

  if (success != GL_TRUE) {
    GLint log_length = 0;
    glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &log_length);

    std::vector<char> log(log_length);
    glGetProgramInfoLog(program_id, log_length, nullptr, log.data());

    ENGINE_ASSERT(false, "shader linking failed: {}", log.data());
  }
}

}

Shader::Shader(std::filesystem::path&& base_path)
  : m_base_path(std::move(base_path)) {

  uint32_t vertex_id = compile_stage(GL_VERTEX_SHADER, m_base_path.string() + ".vert");
  uint32_t fragment_id = compile_stage(GL_FRAGMENT_SHADER, m_base_path.string() + ".frag");

  link_program(vertex_id, fragment_id);

  glDeleteShader(vertex_id);
  glDeleteShader(fragment_id);

  // Set the program's debug label to the shader path for easier identification in render debuggers.
  if (glad_glObjectLabel != nullptr) {
    glad_glObjectLabel(GL_PROGRAM, m_id, static_cast<GLsizei>(m_base_path.string().size()),
                       m_base_path.string().c_str());
  }

  save_active_uniforms();
}

Shader::~Shader() {
  ENGINE_TRACE("deleting shader {}", m_uuid);
  if (m_id != 0)
    glDeleteProgram(m_id);
}

uint32_t Shader::compile_stage(GLenum type, const std::filesystem::path& fp) {
  std::string source_code = load_file(fp);

  uint32_t shader_id = glCreateShader(type);
  const char* src = source_code.c_str();

  glShaderSource(shader_id, 1, &src, nullptr);
  glCompileShader(shader_id);

  check_shader_compilation(shader_id, fp.filename().string());
  return shader_id;
}

void Shader::link_program(uint32_t vertex_id, uint32_t fragment_id) {
  m_id = glCreateProgram();
  glAttachShader(m_id, vertex_id);
  glAttachShader(m_id, fragment_id);
  glLinkProgram(m_id);

  check_program_linking(m_id);
}

void Shader::bind() const { glUseProgram(m_id); }
void Shader::unbind() const { glUseProgram(0); }

GLint Shader::get_uniform_location(std::string_view name) const {
  auto it = m_uniform_lookup.find(std::string(name));
  if (it == m_uniform_lookup.end()) {
    // TODO after we move uniforms to material and uniform buffer objects we can re enable this, it is useful to catch
    // typos and missing uniforms in shaders
    // ENGINE_WARN("uniform '{}' not found in shader '{}'", name, m_name);
    return -1;
  }
  return it->second;
}

void Shader::set_int(std::string_view name, int value) {
  GLint loc = get_uniform_location(name);
  if (loc != -1)
    glUniform1i(loc, value);
}

void Shader::set_float(std::string_view name, float value) {
  GLint loc = get_uniform_location(name);
  if (loc != -1)
    glUniform1f(loc, value);
}

void Shader::set_vec2(std::string_view name, const glm::vec2& v) {
  GLint loc = get_uniform_location(name);
  if (loc != -1)
    glUniform2f(loc, v.x, v.y);
}

void Shader::set_vec3(std::string_view name, const glm::vec3& v) {
  GLint loc = get_uniform_location(name);
  if (loc != -1)
    glUniform3f(loc, v.x, v.y, v.z);
}

void Shader::set_vec4(std::string_view name, const glm::vec4& v) {
  GLint loc = get_uniform_location(name);
  if (loc != -1)
    glUniform4f(loc, v.x, v.y, v.z, v.w);
}

void Shader::set_mat4(std::string_view name, const glm::mat4& m) {
  GLint loc = get_uniform_location(name);
  if (loc != -1)
    glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(m));
}

void Shader::set_bool(std::string_view name, bool value) { set_int(name, value ? 1 : 0); }

void Shader::save_active_uniforms() {
  GLint count = 0;
  glGetProgramInterfaceiv(m_id, GL_UNIFORM, GL_ACTIVE_RESOURCES, &count);

  std::vector<ActiveUniform> uniforms;
  uniforms.reserve(count);

  GLenum props[] = {GL_NAME_LENGTH, GL_TYPE};
  GLint values[2];

  for (GLint i = 0; i < count; ++i) {
    glGetProgramResourceiv(m_id, GL_UNIFORM, i, 2, props, 2, nullptr, values);

    std::vector<char> name_data(values[0]);
    glGetProgramResourceName(m_id, GL_UNIFORM, i, values[0], nullptr, name_data.data());

    std::string name(name_data.data(), values[0] - 1);

    GLint location = glGetUniformLocation(m_id, name.c_str());
    if (location == -1)
      continue; // optimized out

    m_uniform_lookup[name] = location;

    uniforms.push_back(ActiveUniform{.type = static_cast<GLenum>(values[1]), .name = name, .location = location});
  }

  m_active_uniforms = std::move(uniforms);

  print_active_uniforms();
}

std::string_view Shader::uniform_type_to_string(GLenum type) {
  switch (type) {
  case GL_INT: return "int";
  case GL_FLOAT: return "float";
  case GL_FLOAT_VEC2: return "vec2";
  case GL_FLOAT_VEC3: return "vec3";
  case GL_FLOAT_VEC4: return "vec4";
  case GL_FLOAT_MAT4: return "mat4";
  case GL_SAMPLER_2D: return "sampler_2D";
  case GL_SAMPLER_CUBE: return "sampler_cube";
  case GL_BOOL: return "bool";
  default: return "unknown";
  }
}

void Shader::print_active_uniforms() const {
  constexpr std::string_view separator = "=================================";

  ENGINE_BACKTRACE("===== shader {} uniforms =====", m_uuid);

  if (m_active_uniforms.empty()) {
    ENGINE_BACKTRACE("shader '{}' has no active uniforms to print", m_uuid);
    ENGINE_BACKTRACE(separator);
    return;
  }

  for (const auto& u : m_active_uniforms)
    ENGINE_BACKTRACE("uniform {} type: {}", u.name, uniform_type_to_string(u.type));

  ENGINE_BACKTRACE(separator);
}

const std::map<std::string, std::string, NumericComparator>& Shader::to_map() {
  if (!m_info.empty())
    return m_info;

  m_info["type"] = "shader";
  m_info["uuid"] = m_uuid;
  m_info["path"] = m_base_path.string();
  m_info["id"] = std::to_string(m_id);

  for (const auto& u : m_active_uniforms)
    m_info["uniform_" + u.name] = uniform_type_to_string(u.type);

  return m_info;
}

bool Shader::operator==(const Asset& other) const {
  if (typeid(*this) != typeid(other))
    ENGINE_ASSERT(false, "cannot compare shader with other asset type!");

  const Shader& s = static_cast<const Shader&>(other);
  return m_base_path == s.m_base_path && m_id == s.m_id;
}

}