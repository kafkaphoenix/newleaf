#pragma once

#include <memory>

#include <imgui.h>

#include "../assets/assets_manager.h"
#include "../graphics/render_manager.h"
#include "../scene/scene_manager.h"
#include "../settings/settings_manager.h"
#include "im_about.h"
#include "im_logger.h"
#include "im_metrics.h"

namespace nl {

inline void draw_menubar(const std::unique_ptr<AssetsManager>& assets_manager,
                         const std::unique_ptr<RenderManager>& render_manager,
                         const std::unique_ptr<SceneManager>& scene_manager) {
  draw_metrics(assets_manager, render_manager, scene_manager);
  draw_logger();
  draw_about();

  if (ImGui::BeginMenuBar()) {
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