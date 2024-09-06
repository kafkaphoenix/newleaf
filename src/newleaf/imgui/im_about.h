#pragma once

#include <imgui.h>

#include "im_utils.h"

namespace nl {

bool show_tool_about = false;

inline void draw_about() {
  if (not show_tool_about)
    return;

  ImGui::SetNextWindowSize(ImVec2(400, 400), ImGuiCond_FirstUseEver);
  if (ImGui::Begin("About newleaf", &show_tool_about)) {
    ImGui::Text("newleaf is open source and licensed under the MIT license");
    ImGui::Text("Source code and documentation can be found on");
    ImGui::SameLine();
    url("Github", "https://github.com/kafkaphoenix/newleaf.git");
  }
  ImGui::End();
}
}