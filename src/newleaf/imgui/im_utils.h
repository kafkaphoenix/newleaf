#pragma once

#include <imgui.h>

#include <string_view>

namespace nl {

inline void help_mark(std::string_view help_text) {
  ImGui::TextDisabled("(?)");
  if (ImGui::IsItemHovered()) {
    if (ImGui::BeginTooltip()) {
      ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.f);
      ImGui::TextUnformatted(help_text.data());
      ImGui::PopTextWrapPos();
    }
    ImGui::EndTooltip();
  }
}

inline void underline(ImColor color, float thickness = 1.f) {
  ImVec2 min = ImGui::GetItemRectMin();
  ImVec2 max = ImGui::GetItemRectMax();
  min.y = max.y;
  ImGui::GetWindowDrawList()->AddLine(min, max, color, thickness);
}

inline int collapser() {
  int collapsed = -1;

  auto button_color = ImGui::GetStyleColorVec4(ImGuiCol_Button);
  ImGui::PushStyleColor(ImGuiCol_Border, button_color);
  ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.f);
  button_color.w = 0.f;
  ImGui::PushStyleColor(ImGuiCol_Button, button_color);

  if (ImGui::Button("Expand all"))
    collapsed = 1;
  ImGui::SameLine();
  if (ImGui::Button("Collapse all"))
    collapsed = 0;

  ImGui::PopStyleColor(2);
  ImGui::PopStyleVar();

  return collapsed;
}
}