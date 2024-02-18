#pragma once

#include <glm/glm.hpp>

namespace potatoengine {

class Input {
  public:
    static bool is_key_pressed(uint32_t k);
    static bool is_mouse_button_pressed(uint32_t b);
    static glm::vec2 get_mouse_pos();
    static float get_mouse_x();
    static float get_mouse_y();
};
}