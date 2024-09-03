#include "../core/input.h"

#include <GLFW/glfw3.h>

#include "../core/application.h"
#include "../pch.h"

namespace nl {

bool Input::is_key_pressed(uint32_t k) {
  GLFWwindow* w = Application::Get().get_window_manager()->get_native_window();
  return glfwGetKey(w, k) == GLFW_PRESS;
}

bool Input::is_mouse_button_pressed(uint32_t b) {
  GLFWwindow* w = Application::Get().get_window_manager()->get_native_window();
  return glfwGetMouseButton(w, b) == GLFW_PRESS;
}

glm::vec2 Input::get_mouse_pos() {
  GLFWwindow* w = Application::Get().get_window_manager()->get_native_window();
  double xpos, ypos;
  glfwGetCursorPos(w, &xpos, &ypos);

  return glm::vec2((float)xpos, (float)ypos);
}

float Input::get_mouse_x() { return get_mouse_pos().x; }

float Input::get_mouse_y() { return get_mouse_pos().y; }

}