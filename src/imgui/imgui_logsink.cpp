#include "imgui/imgui_logsink.h"

#include <string_view>
#include <chrono>
#include <format>
#include <mutex>
#include <memory>

extern template class spdlog::sinks::base_sink<std::mutex>;
namespace potatoengine {
void ImGuiLogsink::clear() {
  std::unique_lock<std::shared_timed_mutex> lock(m_records_mutex);
  m_records.clear();
  lock.unlock();
}

void ImGuiLogsink::show_log_format_popup() {
  ImGui::MenuItem("Log Format", nullptr, false, false);
  ImGui::Checkbox("Show Time", &m_show_time);
  ImGui::SameLine();
  ImGui::Checkbox("Show Thread", &m_show_thread);
  ImGui::SameLine();
  ImGui::Checkbox("Show Source", &m_show_source);
  ImGui::SameLine();
  ImGui::Checkbox("Show Level", &m_show_level);
}

int ImGuiLogsink::calculate_elapsed_local_time(const Record& r) {
  auto now =
    std::chrono::current_zone()->to_local(std::chrono::system_clock::now());
  auto now_str =
    std::format("{:%T}", std::chrono::floor<std::chrono::seconds>(now));
  std::tm tm_now{};
  std::istringstream ss(now_str);
  ss >> std::get_time(&tm_now, "%H:%M:%S");

  ss = std::istringstream(r.m_time);
  std::tm tm_msg{};
  ss >> std::get_time(&tm_msg, "%H:%M:%S");

  int elapsed = tm_now.tm_hour * 3600 + tm_now.tm_min * 60 + tm_now.tm_sec -
                tm_msg.tm_hour * 3600 - tm_msg.tm_min * 60 - tm_msg.tm_sec;
  return elapsed;
}

void ImGuiLogsink::draw(bool* show_tool_logger) {
  ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_FirstUseEver);
  if (ImGui::Begin("Log", show_tool_logger)) {
    auto button_color = ImGui::GetStyleColorVec4(ImGuiCol_Button);
    ImGui::PushStyleColor(ImGuiCol_Border, button_color);
    button_color.w = 0.f;
    ImGui::PushStyleColor(ImGuiCol_Button, button_color);

    ImGui::SetNextItemWidth(85.f);
    if (ImGui::BeginCombo("##log_level", m_filter_level.c_str())) {
      for (auto& level : m_levels) {
        if (ImGui::Selectable(level.c_str(), level == m_filter_level)) {
          m_filter_level = level;
        }
      }
      ImGui::EndCombo();
    }
    if (ImGui::IsItemHovered()) {
      ImGui::SetTooltip("Filter log level");
    }

    ImGui::SameLine();
    ImGui::SetNextItemWidth(85.f);
    if (ImGui::BeginCombo("##log_time", m_filter_time.c_str())) {
      for (auto& time : m_times) {
        if (ImGui::Selectable(time.c_str(), time == m_filter_time)) {
          m_filter_time = time;
        }
      }
      ImGui::EndCombo();
    }
    if (ImGui::IsItemHovered()) {
      ImGui::SetTooltip("Filter log time");
    }

    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.f);
    ImGui::SameLine();
    if (ImGui::Button("Format") or
        ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_F), false) and
          ImGui::IsKeyDown(ImGui::GetKeyIndex(ImGuiKey_LeftCtrl))) {
      ImGui::OpenPopup("LogFormatPopup");
    }
    if (ImGui::IsItemHovered()) {
      ImGui::SetTooltip("Configure the log format. CTRL+F to toggle");
    }
    if (ImGui::BeginPopup("LogFormatPopup")) {
      show_log_format_popup();
      ImGui::EndPopup();
    }

    ImGui::SameLine();
    bool clear_ = false;
    if (m_autoscroll) {
      clear_ = true;
      ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.5F, 0.5F, 0.5F, 1.f));
    }
    if (ImGui::Button("Auto Scroll") or
        ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_A), false) and
          ImGui::IsKeyDown(ImGui::GetKeyIndex(ImGuiKey_LeftCtrl))) {
      m_autoscroll = not m_autoscroll;
    }
    if (ImGui::IsItemHovered()) {
      ImGui::SetTooltip(
        "Automatically scroll to the bottom of the log. CTRL+A to toggle");
    }
    if (clear_) {
      ImGui::PopStyleColor();
    }

    ImGui::SameLine();
    clear_ = false;
    if (m_wrap) {
      clear_ = true;
      ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.5F, 0.5F, 0.5F, 1.f));
    }
    if (ImGui::Button("Wrap") or
        ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_W), false) and
          ImGui::IsKeyDown(ImGui::GetKeyIndex(ImGuiKey_LeftCtrl))) {
      m_wrap = not m_wrap;
    }
    if (ImGui::IsItemHovered()) {
      ImGui::SetTooltip("Wrap log lines to fit the window. CTRL+W to toggle");
    }
    if (clear_) {
      ImGui::PopStyleColor();
    }

    ImGui::SameLine();
    bool copy = false;
    if (ImGui::Button("Copy") or
        ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_C), false) and
          ImGui::IsKeyDown(ImGui::GetKeyIndex(ImGuiKey_LeftCtrl))) {
      copy = true;
    }
    if (ImGui::IsItemHovered()) {
      ImGui::SetTooltip("Copy the log to the clipboard. CTRL+C to copy");
    }

    ImGui::SameLine();
    if (ImGui::Button("Clear") or
        ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_K), false) and
          ImGui::IsKeyDown(ImGui::GetKeyIndex(ImGuiKey_LeftCtrl))) {
      clear();
    }
    if (ImGui::IsItemHovered()) {
      ImGui::SetTooltip("Clear the log. CTRL+K to clear_");
    }

    ImGui::PopStyleColor(2);
    ImGui::PopStyleVar();

    ImGui::Separator();

    ImGui::InputText("##filter", m_text_filter, IM_ARRAYSIZE(m_text_filter));
    if (ImGui::IsItemHovered()) {
      ImGui::SetTooltip("Filter log messages");
    }
    ImGui::SameLine();
    if (ImGui::Button("Clear Filter")) {
      m_text_filter[0] = '\0';
    }

    if (ImGui::BeginChild("ScrollingRegion", ImVec2(0, 0), false,
                          ImGuiWindowFlags_HorizontalScrollbar)) {
      ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 1));

      if (ImGui::BeginTable(
            "LogTable", 5,
            ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_Borders |
              ImGuiTableFlags_Resizable | ImGuiTableFlags_Hideable)) {
        ImGui::TableSetupColumn("Time", ImGuiTableColumnFlags_WidthFixed, 60.f);
        ImGui::TableSetupColumn("Thread", ImGuiTableColumnFlags_WidthFixed,
                                40.f);
        ImGui::TableSetupColumn("Source", ImGuiTableColumnFlags_WidthFixed,
                                65.f);
        ImGui::TableSetupColumn("Level", ImGuiTableColumnFlags_WidthFixed,
                                60.f);
        ImGui::TableSetupColumn("Message", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableHeadersRow();

        std::shared_lock<std::shared_timed_mutex> lock(m_records_mutex);
        bool empty = m_records.empty();
        if (not empty) {
          empty = true;
          if (copy) {
            ImGui::LogToClipboard();
          }
          for (const auto& record : m_records) {
            if (record.m_level not_eq m_filter_level and
                m_filter_level not_eq "all") {
              continue;
            }
            if (m_filter_time not_eq "all") {
              int elapsed = calculate_elapsed_local_time(record);
              if (elapsed > 1 and m_filter_time == "1s") {
                continue;
              }
              if (elapsed > 5 and m_filter_time == "5s") {
                continue;
              }
              if (elapsed > 10 and m_filter_time == "10s") {
                continue;
              }
              if (elapsed > 30 and m_filter_time == "30s") {
                continue;
              }
              if (elapsed > 60 and m_filter_time == "1m") {
                continue;
              }
              if (elapsed > 300 and m_filter_time == "5m") {
                continue;
              }
              if (elapsed > 600 and m_filter_time == "10m") {
                continue;
              }
              if (elapsed > 1800 and m_filter_time == "30m") {
                continue;
              }
              if (elapsed > 3600 and m_filter_time == "1h") {
                continue;
              }
              if (elapsed > 18000 and m_filter_time == "5h") {
                continue;
              }
              if (elapsed > 36000 and m_filter_time == "10h") {
                continue;
              }
              if (elapsed > 108000 and m_filter_time == "30h") {
                continue;
              }
            }

            if (m_text_filter[0] not_eq '\0' and
                strstr(record.m_msg.c_str(), m_text_filter) == nullptr) {
              continue;
            }

            empty = false;

            if (m_wrap) {
              ImGui::PushTextWrapPos();
              ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
            } else {
              ImGui::PushTextWrapPos(ImGui::GetFontSize() * 50.f);
            }

            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            if (m_show_time) {
              ImGui::TableSetColumnEnabled(0, true);
              ImGui::TextUnformatted(record.m_time.c_str());
            } else {
              ImGui::TableSetColumnEnabled(0, false);
            }
            ImGui::TableNextColumn();
            if (m_show_thread) {
              ImGui::TableSetColumnEnabled(1, true);
              ImGui::TextUnformatted(record.m_thread.c_str());
            } else {
              ImGui::TableSetColumnEnabled(1, false);
            }
            ImGui::TableNextColumn();
            if (m_show_source) {
              ImGui::TableSetColumnEnabled(2, true);
              ImGui::TextUnformatted(record.m_source.c_str());
            } else {
              ImGui::TableSetColumnEnabled(2, false);
            }
            ImGui::TableNextColumn();
            if (m_show_level) {
              ImGui::TableSetColumnEnabled(3, true);
              if (record.m_level == "trace") {
                ImGui::PushStyleColor(ImGuiCol_Text,
                                      ImVec4(0.7F, 0.7F, 0.7F, 1.f));
              } else if (record.m_level == "debug") {
                ImGui::PushStyleColor(ImGuiCol_Text,
                                      ImVec4(1.0F, 1.0F, 1.0F, 1.0F));
                ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg,
                                       IM_COL32(0, 0, 255, 255));
              } else if (record.m_level == "info") {
                ImGui::PushStyleColor(ImGuiCol_Text,
                                      ImVec4(0.f, 1.f, 0.f, 1.f));
              } else if (record.m_level == "warning") {
                ImGui::PushStyleColor(ImGuiCol_Text,
                                      ImVec4(1.f, 1.f, 0.f, 1.f));
              } else if (record.m_level == "error") {
                ImGui::PushStyleColor(ImGuiCol_Text,
                                      ImVec4(1.f, 0.f, 0.f, 1.f));
              } else if (record.m_level == "critical") {
                ImGui::PushStyleColor(ImGuiCol_Text,
                                      ImVec4(1.0F, 1.0F, 1.0F, 1.0F));
                ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg,
                                       IM_COL32(255, 0, 0, 255));
              }
              ImGui::TextUnformatted(record.m_level.c_str());
              ImGui::PopStyleColor();
            } else {
              ImGui::TableSetColumnEnabled(3, false);
            }
            ImGui::TableNextColumn();
            ImGui::TextUnformatted(record.m_msg.c_str());

            if (m_wrap) {
              ImGui::PopStyleVar();
              ImGui::PopTextWrapPos();
            } else {
              ImGui::PopTextWrapPos();
            }
          }
          if (copy) {
            ImGui::LogFinish();
          }
        }
        if (m_autoscroll and not empty and
            ImGui::GetScrollY() < ImGui::GetScrollMaxY()) {
          ImGui::SetScrollHereY(1.f);
        }
        ImGui::EndTable();
        lock.unlock();

        if (empty) {
          std::string text = "No log records";
          auto window_w = ImGui::GetWindowSize().x;
          auto textWidth = ImGui::CalcTextSize(text.c_str()).x;

          ImGui::NewLine();
          ImGui::SetCursorPosX((window_w - textWidth) * 0.5f);
          ImGui::TextUnformatted(text.c_str());
        }
      }

      ImGui::PopStyleVar();
    }
    ImGui::EndChild();
  }

  ImGui::End();
}

void ImGuiLogsink::sink_it_(const spdlog::details::log_msg& msg) {
  Record r;

  if (m_show_time) {
    auto time = std::chrono::current_zone()->to_local(msg.time);
    r.m_time =
      std::format("{:%T}", std::chrono::floor<std::chrono::seconds>(time));
  }

  if (m_show_thread) {
    r.m_thread = std::to_string(msg.thread_id);
  }

  if (m_show_source) {
    r.m_source = msg.logger_name.data();
  }

  if (m_show_level) {
    r.m_level = spdlog::level::to_string_view(msg.level).data();
  }

  r.m_msg = std::string(msg.payload.data(), msg.payload.size());

  std::unique_lock<std::shared_timed_mutex> lock(m_records_mutex);
  m_records.emplace_back(std::move(r));
  lock.unlock();
}

void ImGuiLogsink::flush_() {}
}