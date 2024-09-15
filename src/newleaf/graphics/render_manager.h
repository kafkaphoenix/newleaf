#pragma once

#include <map>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>

#define GLM_FORCE_CTOR_INIT
#include <glm/glm.hpp>

#include "../assets/assets_manager.h"
#include "../utils/numeric_comparator.h"
#include "framebuffer.h"
#include "shader_program.h"
#include "vao.h"

namespace nl {

class RenderManager {
  public:
    void init() const;
    void shutdown();
    void reorder() { m_shouldReorder = true; }
    bool should_reorder() const { return m_shouldReorder; }

    void on_window_resized(uint32_t w, uint32_t h) const;

    void begin_scene(glm::mat4 view, glm::mat4 projection,
                     glm::vec3 cameraPosition);
    void end_scene();

    void
    add_shader_program(std::string&& name,
                       const std::unique_ptr<AssetsManager>& assetsManager);
    void add_framebuffer(std::string&& framebuffer, uint32_t width,
                         uint32_t height, uint32_t buffer_type);
    void delete_framebuffer(std::string_view framebuffer);
    const std::unordered_map<std::string, std::unique_ptr<ShaderProgram>>&
    get_shader_programs() const {
      return m_shader_programs;
    }
    const std::unordered_map<std::string, std::unique_ptr<FBO>>&
    get_framebuffers() const {
      return m_framebuffers;
    }
    const std::unique_ptr<ShaderProgram>&
    get_shader_program(std::string_view shaderProgram);

    void render(const std::shared_ptr<VAO>& vao, const glm::mat4& transform,
                std::string_view shaderProgram);
    void render_framebuffer(const std::shared_ptr<VAO>& vao,
                            std::string_view fbo);
    void render_inside_imgui(const std::shared_ptr<VAO>& vao,
                             std::string_view fbo, std::string_view title,
                             glm::vec2 size, glm::vec2 position,
                             bool fit_to_window);
    uint32_t get_shader_programs_count() const {
      return m_shader_programs.size();
    }
    uint32_t get_framebuffers_count() const { return m_framebuffers.size(); }
    void clear();
    void reset_metrics();
    const std::map<std::string, std::string, NumericComparator>& get_metrics();
    static std::unique_ptr<RenderManager> create();

  private:
    glm::mat4 m_view{};
    glm::mat4 m_projection{};
    glm::vec3 m_camera_position{};
    std::unordered_map<std::string, std::unique_ptr<ShaderProgram>>
      m_shader_programs;
    std::unordered_map<std::string, std::unique_ptr<FBO>> m_framebuffers;
    std::map<std::string, std::string, NumericComparator> m_metrics;
    uint32_t m_draw_calls{};
    uint32_t m_triangles{};
    uint32_t m_vertices{};
    uint32_t m_indices{};
    bool m_shouldReorder{};
};
}