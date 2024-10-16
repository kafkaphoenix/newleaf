#pragma once

#include <imgui.h>

#include "assets/assetsManager.h"
#include "pch.h"
#include "render/renderManager.h"
#include "scene/sceneManager.h"
#include "imgui/imutils.h"

namespace potatoengine {

bool show_tool_metrics = false;

inline void
drawMetrics(const std::unique_ptr<assets::AssetsManager>& assets_manager,
            const std::unique_ptr<RenderManager>& render_manager,
            const std::unique_ptr<SceneManager>& scene_manager) {
  if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_M)) and
      ImGui::IsKeyDown(ImGui::GetKeyIndex(ImGuiKey_LeftCtrl))) {
    show_tool_metrics = not show_tool_metrics;
  }

  if (not show_tool_metrics) {
    return;
  }

  ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_FirstUseEver);
  if (ImGui::Begin("Metrics", &show_tool_metrics)) {
    ImGui::SeparatorText("App");
    auto& io = ImGui::GetIO();
    ImGui::Text("FPS avg %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate,
                io.Framerate);
    ImGui::SameLine();
    underline(ImColor(255, 255, 255));
    helpMark("Frames per second");

    ImGui::SeparatorText("Scene Manager");
    for (const auto& [key, value] : scene_manager->getMetrics()) {
      ImGui::Text("%s: %s", key.c_str(), value.c_str());
    }

    ImGui::SeparatorText("Assets Manager");
    for (const auto& [key, value] : assets_manager->getMetrics()) {
      ImGui::Text("%s: %s", key.c_str(), value.c_str());
    }

    ImGui::SeparatorText("Render Manager");
    for (const auto& [key, value] : render_manager->getMetrics()) {
      ImGui::Text("%s: %s", key.c_str(), value.c_str());
    }

    ImGui::SeparatorText("Hardware");
    ImGui::Text("GPU: %s", glGetString(GL_RENDERER));
    ImGui::Text("OpenGL version: %s", glGetString(GL_VERSION));
    ImGui::Text("GLSL version: %s", glGetString(GL_SHADING_LANGUAGE_VERSION));
  }
  ImGui::End();
}
}