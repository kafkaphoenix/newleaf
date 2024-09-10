#include "imgui_layer.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <imgui_stdlib.h>

#include "../application/application.h"
#include "../imgui/im_debugger.h"
#include "../imgui/im_utils.h"
#include "../logging/log_manager.h"

#include <format>

namespace nl {

ImGuiLayer::ImGuiLayer() : Layer("ImGui layer") {}

void ImGuiLayer::on_attach() {
  ENGINE_TRACE("initializing ImGui layer");
  auto& app = Application::Get();
  const auto& settings_manager = app.get_settings_manager();
  std::string glsl_version =
    std::format("#version {}{}0", settings_manager->opengl_major,
                settings_manager->opengl_minor);
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  (void)io;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
  io.ConfigWindowsMoveFromTitleBarOnly = true;

  ImGui::StyleColorsDark();
  ImGui_ImplGlfw_InitForOpenGL(app.get_window_manager()->get_native_window(),
                               true);
  ImGui_ImplOpenGL3_Init(glsl_version.data());

  ImGuiStyle& style = ImGui::GetStyle();
  style.WindowRounding = 5.f;
  style.FrameRounding = 5.f;
  style.PopupRounding = 5.f;
}

void ImGuiLayer::begin() {
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
}

void ImGuiLayer::end() {
  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void ImGuiLayer::on_imgui_update() {
  auto& app = Application::Get();

  if (app.is_debugging()) {
    const auto& settings_manager = app.get_settings_manager();
    const auto& assets_manager = app.get_assets_manager();
    const auto& render_manager = app.get_render_manager();
    const auto& scene_manager = app.get_scene_manager();
    const auto& states_manager = app.get_states_manager();
    draw_debugger(settings_manager, assets_manager, render_manager,
                  scene_manager, states_manager);
  }
}

void ImGuiLayer::on_detach() {
  ENGINE_WARN("shutting down imgui api");
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
}
}