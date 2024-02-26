#pragma once

#include <GLFW/glfw3.h>

#include "pch.h"

namespace nl {

class OpenGLContext {
  public:
    OpenGLContext(GLFWwindow* w);

    void init();
    void set_context(GLFWwindow* w);
    void swap_buffers();

    static std::unique_ptr<OpenGLContext> Create(GLFWwindow* w);

  private:
    GLFWwindow* m_window{};
};
}