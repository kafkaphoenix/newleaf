#include "input.h"

#include <GLFW/glfw3.h>

#include "../application/application.h"
#include "windows_manager.h"

namespace nl {

// Warning: This is a polling method that calls glfwGetKey() every frame. 
// It works fine for continuous inputs like movement or camera rotation.
// For discrete actions (jump, crouch, shoot), this can trigger repeatedly
// if sticky keys are enabled or loss events if buttons are pressed fast. 
// Use the GLFW event/callback system for one-time input detection and multithreading.
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