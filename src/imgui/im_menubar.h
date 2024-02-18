#pragma once

#include <imgui.h>

#include "assets/assets_manager.h"
#include "core/settings_manager.h"
#include "imgui/im_about.h"
#include "imgui/im_logger.h"
#include "imgui/im_metrics.h"
#include "pch.h"
#include "render/render_manager.h"
#include "scene/scene_manager.h"

namespace potatoengine {

inline void
draw_menubar(const std::unique_ptr<assets::AssetsManager>& assets_manager,
             const std::unique_ptr<RenderManager>& render_manager,
             const std::unique_ptr<SceneManager>& scene_manager,
             const std::unique_ptr<SettingsManager>& settings_manager) {
  draw_metrics(assets_manager, render_manager, scene_manager);
  draw_logger();
  draw_about();

  if (ImGui::BeginMenuBar()) {
    if (ImGui::BeginMenu("Scenes")) {
      for (uint32_t n = 0; n < settings_manager->scenes.size(); n++) {
        if (ImGui::MenuItem(settings_manager->scenes[n], nullptr,
                            settings_manager->active_scene ==
                              settings_manager->scenes[n])) {
          if (settings_manager->active_scene not_eq
              settings_manager->scenes[n]) {
            settings_manager->active_scene = settings_manager->scenes[n];
            settings_manager->active_scene_path =
              settings_manager->scenes_paths[n];
            settings_manager->reload_scene = true;
          }
        }
      }
      ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Tools")) {
      ImGui::MenuItem("Metrics", "CTRL+M", &show_tool_metrics);
      ImGui::MenuItem("Logger", "CTRL+L", &show_tool_logger);
      ImGui::MenuItem("About", NULL, &show_tool_about);
      ImGui::EndMenu();
    }
    ImGui::EndMenuBar();
  }
}
}