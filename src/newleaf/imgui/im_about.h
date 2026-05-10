#pragma once

#include <imgui.h>

namespace nl {

bool show_tool_about = false;

inline void draw_about() {
  if (not show_tool_about)
    return;

  ImGui::SetNextWindowSize(ImVec2(300, 600), ImGuiCond_FirstUseEver);
  if (ImGui::Begin("about newleaf", &show_tool_about)) {
    ImGui::Text("newleaf is open source and licensed under the MIT license");
    ImGui::Text("source code and documentation can be found on");
    ImGui::TextLinkOpenURL("github", "https://github.com/kafkaphoenix/newleaf.git");
  }
  ImGui::End();
}
}