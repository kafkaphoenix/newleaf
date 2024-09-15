#pragma once

#include <map>
#include <memory>
#include <string>
#include <string_view>

#include "../assets/texture.h"
#include "../utils/numeric_comparator.h"

namespace nl {

class FBO {
  public:
    FBO(uint32_t width, uint32_t height, uint32_t buffer_type);
    ~FBO();

    void attach_texture();
    void attach_depth_texture();
    void attach_depth_render_buffer();
    void attach_stencil_render_buffer();
    void attach_depth_stencil_render_buffer();
    void bind_to_draw();
    void bind_to_read();
    void unbind();
    void clear(const float color[4], const float depth);

    void resize(uint32_t width, uint32_t height);
    const std::unique_ptr<Texture>& get_color_texture() const {
      return m_color_texture;
    }
    const std::unique_ptr<Texture>& get_depth_texture() const {
      return m_depth_texture;
    }
    uint32_t get_buffer_id() const;
    std::string_view get_buffer_type() const;
    const std::map<std::string, std::string, NumericComparator>& to_map();

    static std::unique_ptr<FBO> create(uint32_t width, uint32_t height,
                                       uint32_t buffer_type);

    static constexpr uint32_t DEPTH_TEXTURE = 1;
    static constexpr uint32_t DEPTH_RENDERBUFFER = 2;
    static constexpr uint32_t STENCIL_RENDERBUFFER = 3;
    static constexpr uint32_t DEPTH_STENCIL_RENDERBUFFER = 4;

  private:
    uint32_t m_id{};
    uint32_t m_width{};
    uint32_t m_height{};
    uint32_t m_depth_buffer_type{};
    std::unique_ptr<Texture> m_color_texture;
    std::unique_ptr<Texture> m_depth_texture;
    uint32_t m_depth_render_buffer{};
    uint32_t m_stencil_render_buffer{};
    uint32_t m_depth_stencil_render_buffer{};

    std::map<std::string, std::string, NumericComparator> m_info;
};
}