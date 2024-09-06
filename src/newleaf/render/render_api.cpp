#include "render_api.h"

#include <glad/glad.h>

#include <format>
#include <string>
#include <string_view>

#include "../core/log_manager.h"

namespace nl {

void APIENTRY message_callback(GLenum source, GLenum type, uint32_t id,
                               GLenum severity, GLsizei, GLchar const* msg,
                               void const*) {
  std::string_view _source = [source]() {
    switch (source) {
    case GL_DEBUG_SOURCE_API: return "API";
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM: return "WINDOW SYSTEM";
    case GL_DEBUG_SOURCE_SHADER_COMPILER: return "SHADER COMPILER";
    case GL_DEBUG_SOURCE_THIRD_PARTY: return "THIRD PARTY";
    case GL_DEBUG_SOURCE_APPLICATION: return "APPLICATION";
    case GL_DEBUG_SOURCE_OTHER: return "OTHER";
    default: return "UNDEFINED SOURCE";
    }
  }();

  std::string_view _type = [type]() {
    switch (type) {
    case GL_DEBUG_TYPE_ERROR: return "ERROR";
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: return "DEPRECATED_BEHAVIOR";
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: return "UNDEFINED_BEHAVIOR";
    case GL_DEBUG_TYPE_PORTABILITY: return "PORTABILITY";
    case GL_DEBUG_TYPE_PERFORMANCE: return "PERFORMANCE";
    case GL_DEBUG_TYPE_MARKER: return "MARKER";
    case GL_DEBUG_TYPE_OTHER: return "OTHER";
    default: return "UNDEFINED TYPE";
    }
  }();

  std::string_view _severity = [severity]() {
    switch (severity) {
    case GL_DEBUG_SEVERITY_NOTIFICATION: return "NOTIFICATION";
    case GL_DEBUG_SEVERITY_LOW: return "LOW";
    case GL_DEBUG_SEVERITY_MEDIUM: return "MEDIUM";
    case GL_DEBUG_SEVERITY_HIGH: return "HIGH";
    default: return "UNDEFINED SEVERITY";
    }
  }();

  std::string info =
    std::format("source[{}]| type [{}]| severity [{}]| id [{}]| message: {}\n",
                _source, _type, _severity, id, msg);

  switch (severity) {
  case GL_DEBUG_SEVERITY_HIGH: ENGINE_CRITICAL(info); return;
  case GL_DEBUG_SEVERITY_MEDIUM: ENGINE_ERROR(info); return;
  case GL_DEBUG_SEVERITY_LOW: ENGINE_WARN(info); return;
  case GL_DEBUG_SEVERITY_NOTIFICATION: ENGINE_TRACE(info); return;
  }
}

void RenderAPI::init() {
  glEnable(GL_DEBUG_OUTPUT);
  glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
  glDebugMessageCallback(message_callback, nullptr);

  glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE,
                        GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);

  glEnable(GL_CULL_FACE); // BACK FACE CULLING CCW
  glCullFace(GL_BACK);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
  glEnable(GL_LINE_SMOOTH);
  glEnable(GL_DEPTH_TEST);
}

void RenderAPI::toggle_culling(bool enabled) {
  enabled ? glEnable(GL_CULL_FACE) : glDisable(GL_CULL_FACE);
}

void RenderAPI::set_viewport(uint32_t x, uint32_t y, uint32_t w, uint32_t h) {
  glViewport(x, y, w, h);
}

void RenderAPI::set_clear_color(const std::array<float, 4>& color) {
  glClearColor(color[0], color[1], color[2], color[3]);
}

void RenderAPI::set_clear_depth(const float depth) { glClearDepth(depth); }

void RenderAPI::toggle_wireframe(bool enabled) {
  enabled ? glPolygonMode(GL_FRONT_AND_BACK, GL_LINE)
          : glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void RenderAPI::blend(bool enabled) {
  enabled ? glEnable(GL_BLEND) : glDisable(GL_BLEND);
}

void RenderAPI::toggle_depth_test(bool enabled) {
  enabled ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
}

void RenderAPI::set_depth_equal() {
  glDepthFunc(GL_LEQUAL); // depth test passes when values are equal to depth
                          // buffer content [for cubemaps]
}

void RenderAPI::set_depth_less() {
  glDepthFunc(GL_LESS); // default
}

void RenderAPI::clear() { glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); }

void RenderAPI::clear_color() { glClear(GL_COLOR_BUFFER_BIT); }

void RenderAPI::clear_depth() { glClear(GL_DEPTH_BUFFER_BIT); }

void RenderAPI::draw_indexed(const std::shared_ptr<VAO>& vao) {
  vao->bind();
  glDrawElements(GL_TRIANGLES, vao->get_ebo()->get_count(), GL_UNSIGNED_INT,
                 nullptr);
  vao->unbind();
}

}