#pragma once

#include <memory>

#include <imgui.h>

#include "../application/application.h"
#include "../assets/assets_manager.h"
#include "../graphics/render_manager.h"
#include "../scene/scene_manager.h"
#include "../settings/settings_manager.h"
#include "../state/states_manager.h"
#include "im_managersbar.h"
#include "im_menubar.h"

namespace nl {

inline void
draw_debugger(const std::unique_ptr<SettingsManager>& settings_manager,
              const std::unique_ptr<AssetsManager>& assets_manager,
              const std::unique_ptr<RenderManager>& render_manager,
              const std::unique_ptr<SceneManager>& scene_manager,
              const std::unique_ptr<StatesManager>& states_manager) {
  ImGui::SetNextWindowPos(ImVec2(settings_manager->window_w / 2.f, 20.f),
                          ImGuiCond_FirstUseEver);

  ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar;

  ImGui::SetNextWindowSize(ImVec2(500, 600), ImGuiCond_FirstUseEver);
  if (ImGui::Begin("Debugger", nullptr, window_flags)) {
    draw_menubar(assets_manager, render_manager, scene_manager);
    draw_managersbar(assets_manager, render_manager, scene_manager,
                     settings_manager, states_manager);
  }
  ImGui::End();
}
}