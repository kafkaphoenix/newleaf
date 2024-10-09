#include "opengl_context.h"

#include <glad/glad.h>

#include "../logging/log_manager.h"
#include "../utils/assert.h"

namespace nl {

OpenGLContext::OpenGLContext(GLFWwindow* w) : m_window(w) {
  ENGINE_ASSERT(w, "window is null!");
}

void OpenGLContext::init() {
  set_context(m_window);

  ENGINE_ASSERT(gladLoadGLLoader(
                  reinterpret_cast<GLADloadproc>(glfwGetProcAddress)) not_eq 0,
                "failed to initialize glad!");
}

void OpenGLContext::set_context(GLFWwindow* w) {
  // TODO check with multi window if this works
  glfwMakeContextCurrent(w);
}

void OpenGLContext::swap_buffers() { glfwSwapBuffers(m_window); }

std::unique_ptr<OpenGLContext> OpenGLContext::create(GLFWwindow* w) {
  return std::make_unique<OpenGLContext>(w);
}
}