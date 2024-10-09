#pragma once

#include <glm/glm.hpp>

#include "../window/key_codes.h"
#include "../window/mouse_codes.h"

namespace nl {

class Input {
  public:
    static bool is_key_pressed(Key k);
    static bool is_mouse_button_pressed(Mouse b);
    static glm::vec2 get_mouse_pos();
    static float get_mouse_x();
    static float get_mouse_y();
};
}