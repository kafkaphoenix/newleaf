#pragma once

#include <memory>
#include <vector>

#include "mesh.h"
#include "vao.h"

namespace nl {

class ShapeFactory {
  public:
    static Mesh create_triangle(float size);
    static Mesh create_rectangle(float width, float height, bool repeat_texture);
    static Mesh create_cube(float width, float height, float depth, bool repeat_texture);
    static Mesh create_circle(float radius, uint32_t segments);

  private:
    static Mesh create(const std::vector<ShapeVertex>& vertices, const std::vector<uint32_t>& indices);
};
}