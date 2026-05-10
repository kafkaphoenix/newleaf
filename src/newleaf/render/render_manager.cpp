#include "render_manager.h"

#include <numeric>

#include "../application/application.h"
#include "../assets/assets_manager.h"
#include "../assets/texture.h"
#include "../imgui/im_scene.h"
#include "../logging/log_manager.h"
#include "../utils/assert.h"
#include "render_api.h"

namespace nl {

RenderManager::RenderManager() {
  ENGINE_TRACE("initializing render manager");
  RenderAPI::init();
}

RenderManager::~RenderManager() { ENGINE_TRACE("deleting render manager"); }

void RenderManager::on_window_resized(uint32_t w, uint32_t h) const { RenderAPI::set_viewport(0, 0, w, h); }

void RenderManager::begin_scene(glm::mat4 view, glm::mat4 projection, glm::vec3 camera_position) {
  m_view = view;
  m_projection = projection;
  m_camera_position = camera_position;
}

void RenderManager::end_scene() {}

void RenderManager::add_framebuffer(std::string&& name, uint32_t w, uint32_t h, uint32_t t) {
  m_framebuffers.emplace(std::move(name), FBO::create(w, h, t));
}

void RenderManager::delete_framebuffer(std::string_view name) { m_framebuffers.erase(name.data()); }

// TODO redo this when updating fbo
void RenderManager::render_framebuffer(const VAO& vao, const AssetHandle<Shader>& shader, std::string_view fbo,
                                       const std::optional<ImGuiParams>& imgui) {
  auto& scene_fbo = m_framebuffers.at(fbo.data());

  if (imgui) {
    uint32_t w = scene_fbo->get_color_texture().get_width();
    uint32_t h = scene_fbo->get_color_texture().get_height();

    // Create or recreate post-process FBO if size changed
    if (!m_postprocess_fbo || m_postprocess_fbo->get_color_texture().get_width() != w ||
        m_postprocess_fbo->get_color_texture().get_height() != h) {
      m_postprocess_fbo = FBO::create(w, h, FBO::DEPTH_RENDERBUFFER);
    }

    m_postprocess_fbo->bind_to_draw();
    RenderAPI::clear();
  }

  Shader& sp = *shader.get();
  sp.bind();
  sp.set_int("screen_texture", 100);
  scene_fbo->get_color_texture().bind(100);
  RenderAPI::draw_indexed(vao);
  sp.unbind();

  if (imgui) {
    m_postprocess_fbo->unbind();
    render_scene(m_postprocess_fbo->get_color_texture().get_id(), imgui->title, imgui->size, imgui->position,
                 imgui->fit_to_window);
  }

  update_metrics(vao);
}

void RenderManager::render(const VAO& vao, const glm::mat4& transform, const AssetHandle<Shader>& shader) {
  Shader& sp = *shader.get();
  sp.bind();
  sp.set_mat4("projection", m_projection);
  sp.set_mat4("view", m_view);
  sp.set_mat4("model", transform);
  sp.set_vec3("camera_position", m_camera_position);

  RenderAPI::draw_indexed(vao);

  sp.unbind(); // DONT unbind before the draw call
  update_metrics(vao);
}

void RenderManager::clear() {
  ENGINE_TRACE("clearing render manager");
  if (not m_framebuffers.empty()) {
    m_framebuffers.clear();
    // to avoid problems after using scenes with fbo FIXME checks this after fbo refactor
    RenderAPI::set_depth_test(true);
  }
}

std::unique_ptr<RenderManager> RenderManager::create() { return std::make_unique<RenderManager>(); }

void RenderManager::update_metrics(const VAO& vao) {
  m_draw_calls++;
  m_triangles += vao.get_ibo().get_count() / 3;
  m_vertices += vao.get_vbo().get_count();
  m_indices += vao.get_ibo().get_count();
}

std::map<std::string, std::string, NumericComparator>& RenderManager::compute_metrics() {
  m_metrics["framebuffers"] = std::to_string(m_framebuffers.size());
  m_metrics["draw_calls"] = std::to_string(m_draw_calls);
  m_metrics["triangles"] = std::to_string(m_triangles);
  m_metrics["vertices"] = std::to_string(m_vertices);
  m_metrics["indices"] = std::to_string(m_indices);

  return m_metrics;
}

void RenderManager::reset_metrics() {
  m_draw_calls = 0;
  m_triangles = 0;
  m_vertices = 0;
  m_indices = 0;
  m_metrics.clear();
}
}