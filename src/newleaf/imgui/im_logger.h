#pragma once

#include <imgui.h>

#include "../core/log_manager.h"
#include "../imgui/imgui_logsink.h"
#include "../pch.h"

namespace nl {

bool show_tool_logger = false;

inline void draw_logger() {
  if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_L)) and
      ImGui::IsKeyDown(ImGui::GetKeyIndex(ImGuiKey_LeftCtrl))) {
    show_tool_logger = not show_tool_logger;
  }

  if (not show_tool_logger)
    return;

  auto sink = dynamic_cast<ImGuiLogsink*>(
    LogManager::get_engine_logger()->sinks()[1].get());
  sink->draw(&show_tool_logger);
}
}
