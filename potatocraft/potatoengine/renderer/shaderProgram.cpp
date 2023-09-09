#include "potatoengine/renderer/shaderProgram.h"

#include <glm/gtc/type_ptr.hpp>

namespace potatoengine {
ShaderProgram::ShaderProgram(const std::string &name) : m_id(glCreateProgram()), m_name(name) {
}

ShaderProgram::~ShaderProgram() {
    glDeleteProgram(m_id);
}

ShaderProgram::operator GLuint() const {
    return static_cast<GLuint>(m_id);
}

void ShaderProgram::attach(const Shader &s) {
    glAttachShader(m_id, s);
}

void ShaderProgram::detach(const Shader &s) {
    glDetachShader(m_id, s);
}

void ShaderProgram::link() {
    glLinkProgram(m_id);

    int status = GL_FALSE;
    glGetProgramiv(m_id, GL_LINK_STATUS, &status);
    if (status not_eq GL_TRUE) [[unlikely]] {
        int infoLogLength = 0;
        glGetProgramiv(m_id, GL_INFO_LOG_LENGTH, &infoLogLength);
        if (infoLogLength == 0) {
            throw std::runtime_error("Could not link shader program " + m_name);
        }
        std::vector<GLchar> shaderProgramInfoLog(infoLogLength);
        glGetProgramInfoLog(m_id, infoLogLength, &infoLogLength, shaderProgramInfoLog.data());
        throw std::runtime_error("Could not link shader program " + m_name + ": \n" + shaderProgramInfoLog.data());
    }
    m_activeUniforms = getActiveUniforms();
}

void ShaderProgram::use() {
    glUseProgram(m_id);
}

void ShaderProgram::unuse() {
    glUseProgram(0);
}

void ShaderProgram::setInt(std::string_view name, int value) {
    glUniform1i(glGetUniformLocation(m_id, name.data()), value);
}

void ShaderProgram::setFloat(std::string_view name, float value) {
    glUniform1f(glGetUniformLocation(m_id, name.data()), value);
}

void ShaderProgram::setVec2(std::string_view name, const glm::vec2 &vec) {
    glUniform2f(glGetUniformLocation(m_id, name.data()), vec.x, vec.y);
}

void ShaderProgram::setVec3(std::string_view name, const glm::vec3 &vec) {
    glUniform3f(glGetUniformLocation(m_id, name.data()), vec.x, vec.y, vec.z);
}

void ShaderProgram::setVec4(std::string_view name, const glm::vec4 &vec) {
    glUniform4f(glGetUniformLocation(m_id, name.data()), vec.x, vec.y, vec.z, vec.w);
}

void ShaderProgram::setMat4(std::string_view name, const glm::mat4 &mat) {
    glUniformMatrix4fv(glGetUniformLocation(m_id, name.data()), 1, GL_FALSE, glm::value_ptr(mat));
}

std::vector<ActiveUniform> ShaderProgram::getActiveUniforms() {
    GLint numActiveUniforms = 0;
    glGetProgramInterfaceiv(m_id, GL_UNIFORM, GL_ACTIVE_RESOURCES, &numActiveUniforms);

    std::vector<ActiveUniform> activeUniforms;

    std::vector<GLenum> properties;
    properties.push_back(GL_NAME_LENGTH);
    properties.push_back(GL_TYPE);
    properties.push_back(GL_ARRAY_SIZE);
    std::vector<GLint> values(properties.size());

    std::vector<GLchar> nameData(256);

    for (int i = 0; i < numActiveUniforms; ++i) {
        glGetProgramResourceiv(m_id, GL_UNIFORM, i, properties.size(), &properties[0], values.size(), nullptr, &values[0]);

        nameData.resize(values[0]);
        glGetProgramResourceName(m_id, GL_UNIFORM, i, nameData.size(), nullptr, &nameData[0]);
        std::string name(reinterpret_cast<char *>(&nameData[0]), values[0] - 1);

        ActiveUniform uniform;
        uniform.type = values[1];
        uniform.name = name;

        activeUniforms.push_back(uniform);
    }

    return activeUniforms;
}

void ShaderProgram::resetActiveUniforms() {
    use();
    for (const auto &[type, name] : m_activeUniforms) {
        if (type == GL_INT) {
            setInt(name, 0);
        } else if (type == GL_FLOAT) {
            setFloat(name, 0.f);
        } else if (type == GL_FLOAT_VEC2) {
            setVec2(name, glm::vec2(0.f));
        } else if (type == GL_FLOAT_VEC3) {
            setVec3(name, glm::vec3(0.f));
        } else if (type == GL_FLOAT_VEC4) {
            setVec4(name, glm::vec4(0.f));
        } else if (type == GL_FLOAT_MAT4) {
            setMat4(name, glm::mat4(1.f));
        } else if (type == GL_SAMPLER_2D) {
            setInt(name, 0);
        } else if (type == GL_SAMPLER_CUBE) {
            setInt(name, 0);
        } else {
            throw std::runtime_error("Unknown uniform type!");
        }
    }
    unuse();
}

void ShaderProgram::printActiveUniforms() {
    CORE_INFO("================================");
    for (const auto &[type, name] : m_activeUniforms) {
        if (type == GL_INT) {
            CORE_INFO("Active uniform {} type: {}", name, "int");
        } else if (type == GL_FLOAT) {
            CORE_INFO("Active uniform {} type: {}", name, "float");
        } else if (type == GL_FLOAT_VEC2) {
            CORE_INFO("Active uniform {} type: {}", name, "vec2");
        } else if (type == GL_FLOAT_VEC3) {
            CORE_INFO("Active uniform {} type: {}", name, "vec3");
        } else if (type == GL_FLOAT_VEC4) {
            CORE_INFO("Active uniform {} type: {}", name, "vec4");
        } else if (type == GL_FLOAT_MAT4) {
            CORE_INFO("Active uniform {} type: {}", name, "mat4");
        } else if (type == GL_SAMPLER_2D) {
            CORE_INFO("Active uniform {} type: {}", name, "sampler2D");
        } else if (type == GL_SAMPLER_CUBE) {
            CORE_INFO("Active uniform {} type: {}", name, "samplerCube");
        } else {
            throw std::runtime_error("Unknown uniform type!");
        }
    }
}

std::unique_ptr<ShaderProgram> ShaderProgram::Create(const std::string &name) noexcept {
    return std::make_unique<ShaderProgram>(name);
}

}