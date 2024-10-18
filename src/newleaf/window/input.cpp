#include "input.h"

#include <GLFW/glfw3.h>

#include "../application/application.h"
#include "windows_manager.h"

namespace nl {

bool Input::is_key_pressed(Key k) {
  GLFWwindow* w = Application::get().get_windows_manager().get_native_window();
  return glfwGetKey(w, static_cast<uint32_t>(k)) == GLFW_PRESS;
}

bool Input::is_mouse_button_pressed(Mouse b) {
  GLFWwindow* w = Application::get().get_windows_manager().get_native_window();
  return glfwGetMouseButton(w, static_cast<uint32_t>(b)) == GLFW_PRESS;
}

glm::vec2 Input::get_mouse_pos() {
  GLFWwindow* w = Application::get().get_windows_manager().get_native_window();
  double xpos, ypos;
  glfwGetCursorPos(w, &xpos, &ypos);

  return glm::vec2((float)xpos, (float)ypos);
}

float Input::get_mouse_x() { return get_mouse_pos().x; }

float Input::get_mouse_y() { return get_mouse_pos().y; }

}