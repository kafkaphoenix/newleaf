#pragma once

#include <memory>
#include <vector>

#include "vao.h"
#include "../components/graphics/cMesh.h"

namespace nl {

class ShapeFactory {
  public:
    static CMesh create_triangle(float size);
    static CMesh create_rectangle(float width, float height, bool repeat_texture);
    static CMesh create_cube(float width, float height, float depth, bool repeat_texture);
    static CMesh create_circle(float radius, uint32_t segments);

  private:
    static CMesh create(const std::vector<ShapeVertex>& vertices, const std::vector<uint32_t>& indices);
};
}