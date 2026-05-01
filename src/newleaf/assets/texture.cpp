#include "../assets/texture.h"

#include <cmath>
#include <filesystem>
#include <optional>
#include <string>
#include <string_view>

#define STB_IMAGE_IMPLEMENTATION
#define STBI_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image.h>

#include "../logging/log_manager.h"
#include "../utils/assert.h"

namespace nl {

// TODO rethink after SSBO support or if used for mirrors refactor removing parameteri
Texture::Texture(uint32_t width, uint32_t height, GLenum glFormat, std::optional<bool> wrap)
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
  // https://registry.khronos.org/OpenGL-Refpages/gl4/html/glTexStorage2D.xhtml
  if (m_opengl_format == GL_RGBA8) {
    m_format = GL_RGBA;
  } else if (m_opengl_format == GL_DEPTH_COMPONENT24) {
    m_format = GL_DEPTH_COMPONENT;
  } else {
    ENGINE_ASSERT(false, "texture format not supported: {}", m_opengl_format);
  }
}

Texture::Texture(std::filesystem::path&& fp, std::optional<bool> flip_vertically, std::optional<bool> gamma_correction)
  : m_directory(std::filesystem::is_directory(fp) ? std::move(fp.string()) : ""),
    m_cubemap(std::filesystem::is_directory(fp)), m_flip_vertically(flip_vertically.value_or(true)),
    m_gamma_correction(gamma_correction.value_or(false)) {
  if (m_cubemap) {
    std::string file_ext = std::filesystem::exists(fp / "front.jpg") ? ".jpg" : ".png";
    m_paths = default_cubemap_paths(fp, file_ext);
    glGenTextures(1, &m_id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_id);
  } else {
    m_paths.emplace_back(std::move(fp.string()));
    glCreateTextures(GL_TEXTURE_2D, 1, &m_id);
  }

  int width, height, channels;
  stbi_set_flip_vertically_on_load(m_flip_vertically);
  uint32_t face{};
  for (std::string_view path : m_paths) {
    stbi_uc* data = stbi_load(path.data(), &width, &height, &channels, 0);
    ENGINE_ASSERT(data, "failed to load texture: {} {}", path, stbi_failure_reason());
    m_width = width;
    m_height = height;

    m_mipmap_level = calc_mipmap_levels(width, height);

    if (channels == 4) {
      if (m_gamma_correction) {
        m_opengl_format = GL_SRGB8_ALPHA8;
      } else {
        m_opengl_format = GL_RGBA8;
      }
      m_format = GL_RGBA;
    } else if (channels == 3) {
      if (m_gamma_correction) {
        m_opengl_format = GL_SRGB8;
      } else {
        m_opengl_format = GL_RGB8;
      }
      m_format = GL_RGB;
      glPixelStorei(GL_UNPACK_ALIGNMENT, (3 * width % 4 == 0) ? 4 : 1);
    } else if (channels == 2) {
      m_opengl_format = GL_RG8;
      m_format = GL_RG;
    } else if (channels == 1) {
      m_opengl_format = GL_R8;
      m_format = GL_RED;
    } else {
      stbi_image_free(data);
      ENGINE_ASSERT(false, "texture format not supported: {} {} channels", path, channels);
    }

    if (m_cubemap) {
      glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, 0, m_format, m_width, m_height, 0, m_format, GL_UNSIGNED_BYTE,
                   data);
      ++face;
    } else {
      glTextureStorage2D(m_id, m_mipmap_level, m_opengl_format, m_width, m_height);
      glTextureSubImage2D(m_id, 0, 0, 0, m_width, m_height, m_format, GL_UNSIGNED_BYTE, data);
      glGenerateTextureMipmap(m_id);
    }
    stbi_image_free(data);
  }
}

std::vector<std::string> Texture::default_cubemap_paths(const std::filesystem::path& dir, const std::string& ext) {
  // order matters for cubemap to be rendered correctly
  return {(dir / ("front" + ext)).string(),  (dir / ("back" + ext)).string(),  (dir / ("top" + ext)).string(),
          (dir / ("bottom" + ext)).string(), (dir / ("right" + ext)).string(), (dir / ("left" + ext)).string()};
}

// Compute mipmap levels so minified textures sample smaller images, reducing moire/aliasing.
int Texture::calc_mipmap_levels(int width, int height) {
  int size = std::max(width, height);
  return 1 + static_cast<int>(std::floor(std::log2(size)));
}

Texture::~Texture() {
  std::string_view source = (m_paths.size() == 1) ? m_paths[0] : m_directory;
  ENGINE_TRACE("deleting texture {}: {}", m_id, source);
  glDeleteTextures(1, &m_id);
}

void Texture::bind(uint32_t slot) const { glBindTextureUnit(slot, m_id); }

const std::map<std::string, std::string, NumericComparator>& Texture::to_map() {
  if (not m_info.empty()) {
    return m_info;
  }

  m_info["type"] = "texture";
  m_info["id"] = std::to_string(m_id);
  m_info["uuid"] = m_uuid;
  for (uint32_t i = 0; i < m_paths.size(); ++i) {
    m_info["path_" + std::to_string(i)] = m_paths[i];
  }
  m_info["width"] = std::to_string(m_width);
  m_info["height"] = std::to_string(m_height);
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
  m_info["cubemap"] = m_cubemap ? "true" : "false";
  m_info["flip_vertically"] = m_flip_vertically ? "true" : "false";
  m_info["mipmap_level"] = std::to_string(m_mipmap_level);
  m_info["gamma_correction"] = m_gamma_correction ? "true" : "false";

  return m_info;
}

bool Texture::operator==(const Asset& other) const {
  if (typeid(*this) not_eq typeid(other)) {
    ENGINE_ASSERT(false, "cannot compare texture with other asset type!");
  }
  const Texture& other_texture = static_cast<const Texture&>(other);
  for (const std::string& path : m_paths) {
    if (std::find(other_texture.m_paths.begin(), other_texture.m_paths.end(), path) == other_texture.m_paths.end()) {
      return false;
    }
  }
  return m_id == other_texture.m_id;
}

std::unique_ptr<Texture> Texture::create(uint32_t width, uint32_t height, GLenum glFormat, std::optional<bool> wrap) {
  return std::make_unique<Texture>(width, height, glFormat, wrap);
}
}