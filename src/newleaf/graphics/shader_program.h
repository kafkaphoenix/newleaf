#pragma once

#include <map>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include <glm/glm.hpp>

#include "../assets/shader.h"
#include "../utils/numeric_comparator.h"

namespace nl {

struct ActiveUniform {
    uint32_t type;
    std::string name;
};

class ShaderProgram {
  public:
    ShaderProgram(std::string&& name);
    ~ShaderProgram();

    void attach(const Shader& s);
    void detach(const Shader& s);
    void link();
    void use();
    void unuse();
    void set_int(std::string_view name, int value);
    void set_float(std::string_view name, float value);
    void set_vec2(std::string_view name, const glm::vec2& v);
    void set_vec3(std::string_view name, const glm::vec3& v);
    void set_vec4(std::string_view name, const glm::vec4& v);
    void set_mat4(std::string_view name, const glm::mat4& m);
    void reset_active_uniforms();
    void print_active_uniforms();
    const std::map<std::string, std::string, NumericComparator>& to_map();

    std::string_view get_name() const { return m_name; }
    uint32_t get_id() const { return m_id; }
    operator GLuint() const;

    static std::unique_ptr<ShaderProgram> create(std::string&& name);

  private:
    uint32_t m_id{};
    std::string m_name;
    std::vector<ActiveUniform> m_active_uniforms;
    std::map<std::string, std::string, NumericComparator> m_info;

    std::vector<ActiveUniform> get_active_uniforms();
};

}