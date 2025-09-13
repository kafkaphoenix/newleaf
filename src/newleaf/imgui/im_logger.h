#pragma once

#include <imgui.h>

#include "../logging/imgui_logsink.h"
#include "../logging/log_manager.h"

namespace nl {

bool show_tool_logger = false;

inline void draw_logger() {
  if (ImGui::IsKeyPressed(ImGuiKey_L) and ImGui::IsKeyDown(ImGuiKey_LeftCtrl)) {
    show_tool_logger = not show_tool_logger;
  }

  if (not show_tool_logger)
    return;

  ImGuiLogsink* sink = dynamic_cast<ImGuiLogsink*>(LogManager::get_engine_logger().sinks()[1].get());
  sink->draw(&show_tool_logger);
}
}
