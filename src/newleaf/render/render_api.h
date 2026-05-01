#pragma once

#include "vao.h"
#include <array>

namespace nl {
class RenderAPI {
  public:
    enum class DepthFunc { Never, Less, Equal, LessEqual, Greater, NotEqual, GreaterEqual, Always };

    static void init();
    static void draw_indexed(const VAO& vao);
    static void set_wireframe(bool enabled);
    static void set_blend(bool enabled);
    static void set_depth_mask(bool enabled);
    static void set_depth_test(bool enabled);
    static void set_depth_func(DepthFunc func);
    static void set_culling(bool enabled);
    static void set_viewport(uint32_t x, uint32_t y, uint32_t w, uint32_t h);
    static void set_clear_color(const std::array<float, 4>& color);
    static void clear();
    static void clear_color();
    static void clear_depth();
};
}