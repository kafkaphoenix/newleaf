#pragma once

#include "render/vao.h"

namespace potatoengine {

class ShapeFactory {
  public:
    static std::shared_ptr<VAO> create_triangle(float size);
    static std::shared_ptr<VAO> create_rectangle(float width, float height,
                                                 bool repeat_texture);
    static std::shared_ptr<VAO> create_cube(float width, float height,
                                            float depth, bool repeat_texture);
    static std::shared_ptr<VAO> create_circle(float radius, uint32_t segments);

  private:
    static std::shared_ptr<VAO> Create(const std::vector<ShapeVertex>& vertices,
                                       const std::vector<uint32_t>& indices);
};
}