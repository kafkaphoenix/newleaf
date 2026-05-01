#pragma once

#include <filesystem>
#include <map>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "../utils/numeric_comparator.h"
#include "asset.h"
#include "stringhash.h"

namespace nl {

struct ActiveUniform {
    GLenum type{};
    std::string name;
    GLint location{-1};
};

class Shader : public Asset {
public:
    Shader() = delete;
    Shader(std::filesystem::path&& base_path);
    ~Shader() override final;

    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

    void bind() const;
    void unbind() const;

    void set_int(std::string_view name, int value);
    void set_float(std::string_view name, float value);
    void set_vec2(std::string_view name, const glm::vec2& v);
    void set_vec3(std::string_view name, const glm::vec3& v);
    void set_vec4(std::string_view name, const glm::vec4& v);
    void set_mat4(std::string_view name, const glm::mat4& m);
    void set_bool(std::string_view name, bool value);
    
    uint32_t get_id() const { return m_id; }
    operator GLuint() const { return m_id; }
    
    const std::map<std::string, std::string, NumericComparator>& to_map() override final;
    bool operator==(const Asset& other) const override final;
    
    private:
    std::filesystem::path m_base_path;
    uint32_t m_id{};
    std::vector<ActiveUniform> m_active_uniforms;
    std::unordered_map<std::string, GLint, StringHash, std::equal_to<>> m_uniform_lookup;
    std::map<std::string, std::string, NumericComparator> m_info;
    
    uint32_t compile_stage(GLenum type, const std::filesystem::path& fp);
    void link_program(uint32_t vertex_id, uint32_t fragment_id);
    void save_active_uniforms();
    void print_active_uniforms() const;
    GLint get_uniform_location(std::string_view name) const;
    static std::string_view uniform_type_to_string(GLenum type);
};

}