#pragma once

#include <imgui.h>

#include "assets/assets_manager.h"
#include "core/application.h"
#include "core/settings_manager.h"
#include "core/states_manager.h"
#include "imgui/im_managersbar.h"
#include "imgui/im_menubar.h"
#include "pch.h"
#include "render/render_manager.h"
#include "scene/scene_manager.h"

namespace potatoengine {

inline void
draw_debugger(const std::unique_ptr<SettingsManager>& settings_manager,
              const std::unique_ptr<assets::AssetsManager>& assets_manager,
              const std::unique_ptr<RenderManager>& render_manager,
              const std::unique_ptr<SceneManager>& scene_manager,
              const std::unique_ptr<StatesManager>& states_manager) {
  ImGui::SetNextWindowPos(ImVec2(settings_manager->window_w / 2.f, 20.f),
                          ImGuiCond_FirstUseEver);

  ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar;

  ImGui::SetNextWindowSize(ImVec2(500, 600), ImGuiCond_FirstUseEver);
  if (ImGui::Begin("Debugger", nullptr, window_flags)) {
    draw_menubar(assets_manager, render_manager, scene_manager,
                 settings_manager);
    draw_managersbar(assets_manager, render_manager, scene_manager,
                     settings_manager, states_manager);
  }
  ImGui::End();
}
}