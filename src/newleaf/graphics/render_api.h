#pragma once

#include "vao.h"
#include <array>
#include <memory>

namespace nl {
class RenderAPI {
  public:
    static void init();
    static void toggle_culling(bool enabled);
    static void set_viewport(uint32_t x, uint32_t y, uint32_t w, uint32_t h);
    static void set_clear_color(const std::array<float, 4>& color);
    static void set_clear_depth(const float depth);
    static void toggle_wireframe(bool enabled);
    static void blend(bool enabled);
    static void toggle_depth_test(bool enabled);
    static void set_depth_lequal();
    static void set_depth_less();
    static void clear();
    static void clear_color();
    static void clear_depth();
    static void draw_indexed(const std::shared_ptr<VAO>& vao);
};
}