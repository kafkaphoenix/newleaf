#pragma once

#include <glm/glm.hpp>
#include <imgui.h>

#include "../application/application.h"

#include <string_view>

namespace nl {

inline void render_scene(uint32_t fboTextureID, std::string_view title,
                         glm::vec2 size, glm::vec2 position,
                         bool fit_to_window) {
  const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
  uint32_t window_w = size.x != 0 ? size.x : main_viewport->Size.x;
  uint32_t window_h = size.y != 0 ? size.y : main_viewport->Size.y;
  window_w *= 0.75f;
  window_h *= 0.75f;
  uint32_t w, h;
  if (fit_to_window) {
    w = window_w;
    h = window_h;
  } else {
    w = main_viewport->Size.x;
    h = main_viewport->Size.y;
  }
  ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoBringToFrontOnFocus |
                                  ImGuiWindowFlags_NoResize |
                                  ImGuiWindowFlags_NoScrollbar;
  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
  ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_FirstUseEver);
  ImGui::SetNextWindowSize(ImVec2(window_w, window_h));
  if (ImGui::Begin(title.data(), nullptr, window_flags)) {
    ImGui::Image((void*)fboTextureID, ImVec2(w, h), ImVec2(0, 1), ImVec2(1, 0));
  }
  ImGui::PopStyleVar();
  ImGui::End();
}
}