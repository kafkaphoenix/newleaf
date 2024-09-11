#pragma once

#include <GLFW/glfw3.h>

#include <memory>

namespace nl {

class OpenGLContext {
  public:
    OpenGLContext(GLFWwindow* w);

    void init();
    void set_context(GLFWwindow* w);
    void swap_buffers();

    static std::unique_ptr<OpenGLContext> create(GLFWwindow* w);

  private:
    GLFWwindow* m_window{};
};
}