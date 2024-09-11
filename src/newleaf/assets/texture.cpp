#include "../assets/texture.h"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_IMAGE_WRITE_IMPLEMENTATION
#include <filesystem>
#include <optional>
#include <stb_image.h>
#include <string>
#include <string_view>

#include "../logging/log_manager.h"
#include "../utils/assert.h"

namespace nl {

Texture::Texture(uint32_t width, uint32_t height, GLenum glFormat,
                 std::optional<bool> wrap)
  : m_width(width), m_height(height), m_opengl_format(glFormat) {
  glCreateTextures(GL_TEXTURE_2D, 1, &m_id);
  glTextureStorage2D(m_id, 1, m_opengl_format, m_width, m_height);
  glTextureParameteri(m_id, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTextureParameteri(m_id, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  if (wrap.value_or(false)) {
    // prevents edge bleeding fbo texture
    glTextureParameteri(m_id, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(m_id, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  }
  m_mipmap_level = 1;
  m_flip_vertically = false;
  m_paths.emplace_back("fbo texture");
  m_type = "texture_difusse";
  // https://registry.khronos.org/OpenGL-Refpages/gl4/html/glTexStorage2D.xhtml
  if (m_opengl_format == GL_RGBA8) {
    m_format = GL_RGBA;
  } else if (m_opengl_format == GL_DEPTH_COMPONENT24) {
    m_format = GL_DEPTH_COMPONENT;
  } else {
    ENGINE_ASSERT(false, "texture format not supported: {}", m_opengl_format);
  }
}

Texture::Texture(std::filesystem::path&& fp, std::optional<std::string>&& type,
                 std::optional<bool> flipVertically,
                 std::optional<uint32_t> mipmap_level,
                 std::optional<bool> gammaCorrection)
  : m_directory(std::filesystem::is_directory(fp) ? std::move(fp.string())
                                                  : ""),
    m_cubemap(std::filesystem::is_directory(fp)),
    m_type(std::move(type.value_or(""))),
    m_flip_vertically(flipVertically.value_or(true)),
    m_mipmap_level(mipmap_level.value_or(4)),
    m_gamma_correction(gammaCorrection.value_or(false)) {
  if (m_cubemap) {
    std::string fileExt =
      std::filesystem::exists(fp / "front.jpg") ? ".jpg" : ".png";
    m_paths.reserve(6);
    m_paths.emplace_back(
      std::move((fp / ("front" + fileExt))
                  .string())); // it needs to be added in this order
    m_paths.emplace_back(std::move((fp / ("back" + fileExt)).string()));
    m_paths.emplace_back(std::move((fp / ("top" + fileExt)).string()));
    m_paths.emplace_back(std::move((fp / ("bottom" + fileExt)).string()));
    m_paths.emplace_back(std::move((fp / ("right" + fileExt)).string()));
    m_paths.emplace_back(std::move((fp / ("left" + fileExt)).string()));
  } else {
    m_paths.emplace_back(std::move(fp.string()));
  }

  loadTexture();
}

void Texture::loadTexture() {
  int width, height, channels;
  stbi_set_flip_vertically_on_load(m_flip_vertically);
  uint32_t face{};
  if (m_cubemap) {
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
    glTextureParameteri(m_id, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTextureParameteri(m_id, GL_TEXTURE_WRAP_T, GL_REPEAT);
  }
  for (std::string_view path : m_paths) {
    stbi_uc* data = stbi_load(path.data(), &width, &height, &channels, 0);
    if (not data) [[unlikely]] {
      stbi_image_free(data);
      ENGINE_ASSERT(false, "failed to load texture: {} {}", path,
                    stbi_failure_reason());
    }
    m_width = width;
    m_height = height;

    if (channels == 4) {
      m_opengl_format = GL_RGBA8;
      m_format = GL_RGBA;
    } else if (channels == 3) {
      m_opengl_format = GL_RGB8;
      m_format = GL_RGB;
      // https://stackoverflow.com/questions/71284184/opengl-distorted-texture
      if (3 * width % 4 == 0) {
        glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
      } else {
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
      }
    } else if (channels == 2) {
      m_opengl_format = GL_RG8;
      m_format = GL_RG;
    } else if (channels == 1) {
      m_opengl_format = GL_R8;
      m_format = GL_RED;
    } else [[unlikely]] {
      stbi_image_free(data);
      ENGINE_ASSERT(false, "texture format not supported: {} {} channels", path,
                    channels);
    }

    if (m_cubemap) {
      glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, 0, m_format, m_width,
                   m_height, 0, m_format, GL_UNSIGNED_BYTE, data);
      ++face;
    } else {
      glTextureStorage2D(m_id, m_mipmap_level, m_opengl_format, m_width,
                         m_height);
      glTextureSubImage2D(m_id, 0, 0, 0, m_width, m_height, m_format,
                          GL_UNSIGNED_BYTE, data);
      glGenerateTextureMipmap(m_id);
    }
    stbi_image_free(data);
  }
}

Texture::~Texture() {
  std::string_view source = (m_paths.size() == 1) ? m_paths[0] : m_directory;
  ENGINE_WARN("deleting texture {}: {}", m_id, source);
  glDeleteTextures(1, &m_id);
}

void Texture::bind_slot(uint32_t slot) {
  ENGINE_ASSERT(slot > 0, "texture slot {} is not allowed!", slot);
  m_slot = slot;
  glBindTextureUnit(slot, m_id);
}

void Texture::rebind_slot() { glBindTextureUnit(m_slot, m_id); }

void Texture::unbind_slot() {
  glBindTextureUnit(m_slot, 0); // unbind texture from slot
  m_slot = 0;                   // nothing render to this slot
}

const std::map<std::string, std::string, NumericComparator>& Texture::to_map() {
  if (not m_info.empty()) {
    return m_info;
  }

  m_info["type"] = "texture";
  m_info["id"] = std::to_string(m_id);
  for (uint32_t i = 0; i < m_paths.size(); ++i) {
    m_info["path_" + std::to_string(i)] = m_paths[i];
  }
  m_info["width"] = std::to_string(m_width);
  m_info["height"] = std::to_string(m_height);
  m_info["texture_type"] = m_type;
  if (m_opengl_format == GL_RGBA8) {
    m_info["openGL_format"] = "rgba8";
  } else if (m_opengl_format == GL_RGB8) {
    m_info["openGL_format"] = "rgb8";
  } else if (m_opengl_format == GL_RG8) {
    m_info["openGL_format"] = "rg8";
  } else if (m_opengl_format == GL_R8) {
    m_info["openGL_format"] = "r8";
  } else if (m_opengl_format == GL_DEPTH_COMPONENT24) {
    m_info["openGL_format"] = "depth_component24";
  } else {
    m_info["openGL_format"] = "unknown";
  }
  if (m_format == GL_RGBA) {
    m_info["format"] = "rgba";
  } else if (m_format == GL_RGB) {
    m_info["format"] = "rgb";
  } else if (m_format == GL_RG) {
    m_info["format"] = "rg";
  } else if (m_format == GL_RED) {
    m_info["format"] = "red";
  } else if (m_format == GL_DEPTH_COMPONENT) {
    m_info["format"] = "depth_component";
  } else {
    m_info["format"] = "unknown";
  }
  m_info["slot"] =
    std::to_string(m_slot); // will be 0 if not bound except for fbo texture
  m_info["cubemap"] = m_cubemap ? "true" : "false";
  m_info["flip_vertically"] = m_flip_vertically ? "true" : "false";
  m_info["mipmap_level"] = std::to_string(m_mipmap_level);
  m_info["gamma_correction"] = m_gamma_correction ? "true" : "false";

  return m_info;
}

bool Texture::operator==(const Asset& other) const {
  if (typeid(*this) != typeid(other)) {
    ENGINE_ASSERT(false, "cannot compare texture with other asset type!");
  }
  const Texture& otherTexture = static_cast<const Texture&>(other);
  for (const std::string& path : m_paths) {
    if (std::find(otherTexture.m_paths.begin(), otherTexture.m_paths.end(),
                  path) == otherTexture.m_paths.end()) {
      return false;
    }
  }
  return m_id == otherTexture.m_id;
}

std::unique_ptr<Texture> Texture::Create(uint32_t width, uint32_t height,
                                         GLenum glFormat,
                                         std::optional<bool> wrap) {
  return std::make_unique<Texture>(width, height, glFormat, wrap);
}
}