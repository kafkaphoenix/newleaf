#include "../render/render_manager.h"

#include "../assets/texture.h"
#include "../imgui/im_scene.h"
#include "../render/render_api.h"

namespace nl {

void RenderManager::init() const { RenderAPI::init(); }

void RenderManager::shutdown() { ENGINE_WARN("Shutting down render manager"); }

void RenderManager::on_window_resized(uint32_t w, uint32_t h) const {
  RenderAPI::set_viewport(0, 0, w, h);
}

void RenderManager::begin_scene(glm::mat4 view, glm::mat4 projection,
                                glm::vec3 cameraPosition) {
  m_view = view;
  m_projection = projection;
  m_cameraPosition = cameraPosition;
}

void RenderManager::end_scene() {}

void RenderManager::add_shader_program(
  std::string&& name,
  const std::unique_ptr<assets::AssetsManager>& assets_manager) {
  auto newShaderProgram = ShaderProgram::Create(std::string(name));
  const auto& vs = assets_manager->get<assets::Shader>("v" + name);
  const auto& fs = assets_manager->get<assets::Shader>("f" + name);
  newShaderProgram->attach(*vs);
  newShaderProgram->attach(*fs);
  newShaderProgram->link();
  newShaderProgram->detach(*vs);
  newShaderProgram->detach(*fs);
  ENGINE_TRACE("Shader {} linked!", name);
  m_shader_programs.emplace(std::move(name), std::move(newShaderProgram));
}

void RenderManager::add_framebuffer(std::string&& name, uint32_t w, uint32_t h,
                                    uint32_t t) {
  m_framebuffers.emplace(std::move(name), FBO::Create(w, h, t));
}

void RenderManager::delete_framebuffer(std::string_view name) {
  m_framebuffers.erase(name.data());
}

void RenderManager::render_framebuffer(const std::shared_ptr<VAO>& vao,
                                       std::string_view fbo) {
  auto& sp = getShaderProgram("fbo");

  sp->use();
  sp->set_int("screenTexture", 100);
  m_framebuffers.at(fbo.data())->get_color_texture()->bind_slot(100);

  RenderAPI::draw_indexed(vao);

  m_drawCalls++;
  m_triangles += vao->get_ebo()->get_count() / 3;
  for (const auto& vbo : vao->get_vbos()) {
    m_vertices += vbo->get_count();
  }
  m_indices += vao->get_ebo()->get_count();
  sp->unuse();
}

void RenderManager::render_inside_imgui(const std::shared_ptr<VAO>& vao,
                                        std::string_view fbo,
                                        std::string_view title, glm::vec2 size,
                                        glm::vec2 position,
                                        bool fit_to_window) {
  auto& fbo_ = m_framebuffers.at(fbo.data());
  render_scene(fbo_->get_color_texture()->get_id(), title, size, position,
               fit_to_window);

  m_drawCalls++;
  m_triangles += vao->get_ebo()->get_count() / 3;
  for (const auto& vbo : vao->get_vbos()) {
    m_vertices += vbo->get_count();
  }
  m_indices += vao->get_ebo()->get_count();
}

void RenderManager::render(const std::shared_ptr<VAO>& vao,
                           const glm::mat4& transform,
                           std::string_view shaderProgram) {
  auto& sp = getShaderProgram(shaderProgram);

  sp->use();
  sp->set_mat4("projection", m_projection);
  sp->set_mat4("view", m_view);
  sp->set_vec3("cameraPosition", m_cameraPosition);
  sp->set_mat4("model", transform);

  RenderAPI::draw_indexed(vao);

  m_drawCalls++;
  m_triangles += vao->get_ebo()->get_count() / 3;
  for (const auto& vbo : vao->get_vbos()) {
    m_vertices += vbo->get_count();
  }
  m_indices += vao->get_ebo()->get_count();
  sp->unuse(); // DONT unuse before the draw call
}

void RenderManager::clear() {
  if (not m_framebuffers.empty()) {
    m_framebuffers.clear();
    // to avoid problems after using scenes with fbo
    RenderAPI::toggle_depth_test(true);
  }
  m_shader_programs.clear();
}

std::unique_ptr<RenderManager> RenderManager::Create() {
  return std::make_unique<RenderManager>();
}

const std::unique_ptr<ShaderProgram>&
RenderManager::getShaderProgram(std::string_view name) {
  ENGINE_ASSERT(m_shader_programs.contains(name.data()),
                "Shader program {} not found!", name);
  return m_shader_programs.at(name.data());
}

const std::map<std::string, std::string, NumericComparator>&
RenderManager::get_metrics() {
  m_metrics["Framebuffers"] = std::to_string(m_framebuffers.size());
  m_metrics["Shader programs"] = std::to_string(m_shader_programs.size());
  m_metrics["Draw calls"] = std::to_string(m_drawCalls);
  m_metrics["Triangles"] = std::to_string(m_triangles);
  m_metrics["Vertices"] = std::to_string(m_vertices);
  m_metrics["Indices"] = std::to_string(m_indices);

  return m_metrics;
}

void RenderManager::reset_metrics() {
  m_drawCalls = 0;
  m_triangles = 0;
  m_vertices = 0;
  m_indices = 0;
  m_metrics.clear();
}
}