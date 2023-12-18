#include "renderer/rendererAPI.h"

#include <glad/glad.h>

namespace potatoengine {

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
    std::format("Source[{}]| Type [{}]| Severity [{}]| ID [{}]| Message: {}\n",
                _source, _type, _severity, id, msg);

  switch (severity) {
  case GL_DEBUG_SEVERITY_HIGH: ENGINE_CRITICAL(info); return;
  case GL_DEBUG_SEVERITY_MEDIUM: ENGINE_ERROR(info); return;
  case GL_DEBUG_SEVERITY_LOW: ENGINE_WARN(info); return;
  case GL_DEBUG_SEVERITY_NOTIFICATION: ENGINE_TRACE(info); return;
  }
}

void RendererAPI::Init() {
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

void RendererAPI::ToggleCulling(bool enabled) {
  enabled ? glEnable(GL_CULL_FACE) : glDisable(GL_CULL_FACE);
}

void RendererAPI::SetViewport(uint32_t x, uint32_t y, uint32_t w, uint32_t h) {
  glViewport(x, y, w, h);
}

void RendererAPI::SetClearColor(const std::array<float, 4>& color) {
  glClearColor(color[0], color[1], color[2], color[3]);
}

void RendererAPI::SetClearDepth(const float depth) { glClearDepth(depth); }

void RendererAPI::ToggleWireframe(bool enabled) {
  enabled ? glPolygonMode(GL_FRONT_AND_BACK, GL_LINE)
          : glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void RendererAPI::ToggleBlend(bool enabled) {
  enabled ? glEnable(GL_BLEND) : glDisable(GL_BLEND);
}

void RendererAPI::ToggleDepthTest(bool enabled) {
  enabled ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
}

void RendererAPI::SetDepthLEqual() {
  glDepthFunc(GL_LEQUAL); // depth test passes when values are equal to depth
                          // buffer content [for cubemaps]
}

void RendererAPI::SetDepthLess() {
  glDepthFunc(GL_LESS); // default
}

void RendererAPI::Clear() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void RendererAPI::ClearColor() { glClear(GL_COLOR_BUFFER_BIT); }

void RendererAPI::ClearDepth() { glClear(GL_DEPTH_BUFFER_BIT); }

void RendererAPI::DrawIndexed(const std::shared_ptr<VAO>& vao) {
  vao->bind();
  glDrawElements(GL_TRIANGLES, vao->getEBO()->getCount(), GL_UNSIGNED_INT,
                 nullptr);
  vao->unbind();
}

}