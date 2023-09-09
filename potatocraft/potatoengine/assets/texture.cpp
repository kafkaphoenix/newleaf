#include "potatoengine/assets/texture.h"

#include <stb_image.h>

#include "potatoengine/pch.h"

namespace potatoengine {

Texture::Texture(const std::filesystem::path& fp, const std::optional<std::string>& type, std::optional<bool> flipVertically, std::optional<int> mipmap_level, std::optional<bool> gammaCorrection)
    : m_directory(std::filesystem::is_directory(fp) ? fp.string() : ""),
      m_isCubemap(std::filesystem::is_directory(fp)),
      m_type(type.value_or("")),
      m_flipVertically(flipVertically.value_or(true)),
      m_mipmap_level(mipmap_level.value_or(4)),
      m_gammaCorrection(gammaCorrection.value_or(false)) {
    if (m_isCubemap) {
        std::string fileExt = std::filesystem::exists(fp / "front.jpg") ? ".jpg" : ".png";
        m_filepaths.push_back((fp / ("front" + fileExt)).string());
        m_filepaths.push_back((fp / ("back" + fileExt)).string());
        m_filepaths.push_back((fp / ("top" + fileExt)).string());
        m_filepaths.push_back((fp / ("bottom" + fileExt)).string());
        m_filepaths.push_back((fp / ("right" + fileExt)).string());
        m_filepaths.push_back((fp / ("left" + fileExt)).string());
    } else {
        m_filepaths.push_back(fp.string());
    }

    loadTexture();
}

void Texture::loadTexture() {
    int width, height, channels;
    stbi_set_flip_vertically_on_load(m_flipVertically);
    uint32_t face{};
    if (m_isCubemap) {
        glGenTextures(1, &m_id);
        glBindTexture(GL_TEXTURE_CUBE_MAP, m_id);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    } else {
        glCreateTextures(GL_TEXTURE_2D, 1, &m_id);
        glTextureParameteri(m_id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTextureParameteri(m_id, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTextureParameteri(m_id, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTextureParameteri(m_id, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }
    for (const std::string& filepath : m_filepaths) {
        stbi_uc* data = stbi_load(filepath.data(), &width, &height, &channels, 0);
        if (not data) [[unlikely]] {
            stbi_image_free(data);
            throw std::runtime_error("Failed to load texture: " + filepath + " " + stbi_failure_reason());
        }
        m_width = width;
        m_height = height;

        if (channels == 4) {
            m_GLFormat = GL_RGBA8;
            m_format = GL_RGBA;
        } else if (channels == 3) {
            m_GLFormat = GL_RGB8;
            m_format = GL_RGB;
        } else if (channels == 2) {
            m_GLFormat = GL_RG8;
            m_format = GL_RG;
        } else if (channels == 1) {
            m_GLFormat = GL_R8;
            m_format = GL_RED;
        } else [[unlikely]] {
            stbi_image_free(data);
            throw std::runtime_error("Texture format not supported: " + std::string(filepath) + " " + std::to_string(channels) + " channels");
        }

        if (m_isCubemap) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, 0, m_format, m_width, m_height, 0, m_format, GL_UNSIGNED_BYTE, data);
            ++face;
        } else {
            glTextureStorage2D(m_id, m_mipmap_level, m_GLFormat, m_width, m_height);
            glTextureSubImage2D(m_id, 0, 0, 0, m_width, m_height, m_format, GL_UNSIGNED_BYTE, data);
            glGenerateTextureMipmap(m_id);
        }
        stbi_image_free(data);
    }
}

Texture::~Texture() {
    glDeleteTextures(1, &m_id);
}

void Texture::bindSlot(uint32_t slot) {
    if (slot == 0) [[unlikely]] {
        throw std::runtime_error("Texture slot 0 is not allowed!");
    }
    m_slot = slot;
    glBindTextureUnit(slot, m_id);
}

void Texture::rebindSlot() {
    glBindTextureUnit(m_slot, m_id);
}

void Texture::unbindSlot() {
    glBindTextureUnit(m_slot, 0); // unbind texture from slot
    m_slot = 0; // nothing render to this slot
}
}