#include "framebuffer.h"

#include <glad/glad.h>

#include "../core/application.h"
#include "../core/log_manager.h"
#include "../utils/map_json_serializer.h"

namespace nl {

FBO::FBO(uint32_t w, uint32_t h, uint32_t t) : m_depth_buffer_type(t) {
  const auto& settings_manager = Application::Get().get_settings_manager();
  uint32_t window_w;
  uint32_t window_h;
  if (settings_manager->fullscreen) {
    int monitorCount;
    GLFWmonitor* monitor =
      (glfwGetMonitors(&monitorCount))[settings_manager->primary_monitor];
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    window_w = mode->width;
    window_h = mode->height;
  } else {
    window_w = settings_manager->window_w;
    window_h = settings_manager->window_h;
  }
  m_width = w == 0 ? window_w : w;
  m_height = h == 0 ? window_h : h;
  glCreateFramebuffers(1, &m_id);
  glNamedFramebufferDrawBuffer(m_id, GL_COLOR_ATTACHMENT0);
  attach_texture();
  if (m_depth_buffer_type == DEPTH_TEXTURE) {
    attach_depth_texture();
  } else if (m_depth_buffer_type == DEPTH_RENDERBUFFER) {
    attach_depth_render_buffer();
  } else if (m_depth_buffer_type == STENCIL_RENDERBUFFER) {
    attach_stencil_render_buffer();
  }
  uint32_t status = glCheckNamedFramebufferStatus(m_id, GL_FRAMEBUFFER);
  ENGINE_ASSERT(status == GL_FRAMEBUFFER_COMPLETE, "framebuffer error: {}",
                status);
}

FBO::~FBO() {
  ENGINE_WARN("deleting framebuffer {}", m_id);
  glDeleteFramebuffers(1, &m_id);
  glDeleteRenderbuffers(1, &m_depth_render_buffer);
  glDeleteRenderbuffers(1, &m_stencil_render_buffer);
}

void FBO::attach_texture() {
  m_color_texture =
    assets::Texture::Create(m_width, m_height, GL_RGBA8, assets::Texture::WRAP);
  glNamedFramebufferTexture(m_id, GL_COLOR_ATTACHMENT0,
                            m_color_texture->get_id(), 0);
}

void FBO::attach_depth_texture() {
  // slower than renderbuffer but can be sampled in shaders
  m_depth_texture = assets::Texture::Create(
    m_width, m_height, GL_DEPTH_COMPONENT24, assets::Texture::DONT_WRAP);
  glNamedFramebufferTexture(m_id, GL_DEPTH_ATTACHMENT,
                            m_depth_texture->get_id(), 0);
}

void FBO::attach_depth_render_buffer() {
  glCreateRenderbuffers(1, &m_depth_render_buffer);
  glNamedRenderbufferStorage(m_depth_render_buffer, GL_DEPTH_COMPONENT24,
                             m_width, m_height);
  glNamedFramebufferRenderbuffer(m_id, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER,
                                 m_depth_render_buffer);
}

void FBO::attach_stencil_render_buffer() {
  glCreateRenderbuffers(1, &m_stencil_render_buffer);
  glNamedRenderbufferStorage(m_stencil_render_buffer, GL_STENCIL_INDEX8,
                             m_width, m_height);
  glNamedFramebufferRenderbuffer(m_id, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER,
                                 m_stencil_render_buffer);
}

void FBO::attach_depth_stencil_render_buffer() {
  glCreateRenderbuffers(1, &m_depth_stencil_render_buffer);
  glNamedRenderbufferStorage(m_depth_stencil_render_buffer, GL_DEPTH24_STENCIL8,
                             m_width, m_height);
  glNamedFramebufferRenderbuffer(m_id, GL_DEPTH_STENCIL_ATTACHMENT,
                                 GL_RENDERBUFFER,
                                 m_depth_stencil_render_buffer);
}

uint32_t FBO::get_buffer_id() const {
  if (m_depth_buffer_type == DEPTH_TEXTURE) {
    return m_depth_texture->get_id();
  } else if (m_depth_buffer_type == DEPTH_RENDERBUFFER) {
    return m_depth_render_buffer;
  } else if (m_depth_buffer_type == STENCIL_RENDERBUFFER) {
    return m_stencil_render_buffer;
  } else if (m_depth_buffer_type == DEPTH_STENCIL_RENDERBUFFER) {
    return m_depth_stencil_render_buffer;
  } else {
    ENGINE_ERROR("no buffer attached to framebuffer {}", m_id);
    return 0;
  }
}

std::string_view FBO::get_buffer_type() const {
  if (m_depth_buffer_type == DEPTH_TEXTURE) {
    return "depth texture";
  } else if (m_depth_buffer_type == DEPTH_RENDERBUFFER) {
    return "depth renderbuffer";
  } else if (m_depth_buffer_type == STENCIL_RENDERBUFFER) {
    return "stencil renderbuffer";
  } else if (m_depth_buffer_type == DEPTH_STENCIL_RENDERBUFFER) {
    return "depth stencil renderbuffer";
  } else {
    ENGINE_ERROR("no buffer attached to framebuffer {}", m_id);
    return "none";
  }
}

const std::map<std::string, std::string, NumericComparator>& FBO::to_map() {
  if (not m_info.empty()) {
    return m_info;
  }

  m_info["type"] = "fbo";
  m_info["id"] = std::to_string(m_id);
  m_info["width"] = std::to_string(m_width);
  m_info["height"] = std::to_string(m_height);
  m_info["buffer_id"] = std::to_string(get_buffer_id());
  m_info["buffer_type"] = get_buffer_type();
  if (m_color_texture) {
    m_info["color_texture"] = map_to_json(m_color_texture->to_map());
  }
  if (m_depth_texture) {
    m_info["depth_texture"] = map_to_json(m_depth_texture->to_map());
  }

  return m_info;
}

void FBO::bind_to_draw() {
  const auto& render_manager = Application::Get().get_render_manager();
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_id);
  render_manager->on_window_resized(m_width, m_height);
}

void FBO::bind_to_read() {
  m_color_texture->bind_slot(100);
  glBindFramebuffer(GL_READ_FRAMEBUFFER, m_id);
  glNamedFramebufferReadBuffer(m_id, GL_COLOR_ATTACHMENT0);
}

void FBO::unbind() {
  glBindFramebuffer(GL_FRAMEBUFFER, 0); // default framebuffer
  auto& app = Application::Get();
  const auto& settings_manager = app.get_settings_manager();
  const auto& render_manager = app.get_render_manager();
  if (settings_manager->fullscreen) {
    int monitorCount;
    GLFWmonitor* monitor =
      (glfwGetMonitors(&monitorCount))[settings_manager->primary_monitor];
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    render_manager->on_window_resized(mode->width, mode->height);
  } else {
    render_manager->on_window_resized(settings_manager->window_w,
                                      settings_manager->window_h);
  }
}

void FBO::clear(const float color[4], const float depth) {
  glClearNamedFramebufferfv(m_id, GL_COLOR, 0, color);
  glClearNamedFramebufferfv(m_id, GL_DEPTH, 0, &depth);
}

void FBO::resize(uint32_t width, uint32_t height) {
  m_width = width;
  m_height = height;
  bind_to_draw();
}

std::unique_ptr<FBO> FBO::Create(uint32_t width, uint32_t height,
                                 uint32_t buffer_type) {
  return std::make_unique<FBO>(width, height, buffer_type);
}
}