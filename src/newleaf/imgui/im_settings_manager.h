#pragma once

#include <format>
#include <memory>
#include <string>

#include <imgui.h>
#include <imgui_stdlib.h>

#include "../application/application.h"
#include "../graphics/render_manager.h"
#include "../logging/log_manager.h"
#include "../settings/settings_manager.h"
#include "../window/windows_manager.h"
#include "im_utils.h"

namespace nl {

std::string selected_settings_manager_tabkey;

inline void draw_settings_manager(SettingsManager& settings_manager,
                                  const RenderManager& render_manager) {
  ImGui::Columns(2);

  ImGui::SeparatorText("Engine");
  if (ImGui::Selectable("Window")) {
    selected_settings_manager_tabkey = "Window";
  }
  if (ImGui::Selectable("Debug")) {
    selected_settings_manager_tabkey = "Debug";
  }
  if (ImGui::Selectable("Logger")) {
    selected_settings_manager_tabkey = "Logger";
  }
  if (ImGui::Selectable("Render")) {
    selected_settings_manager_tabkey = "Render";
  }
  if (ImGui::Selectable("Scene")) {
    selected_settings_manager_tabkey = "Scene";
  }

  auto& app = Application::get();
  bool paused = app.is_paused();

  ImGui::NextColumn();
  if (not selected_settings_manager_tabkey.empty()) {
    ImGui::SeparatorText("Edit");
    if (selected_settings_manager_tabkey == "Window") {
      auto& windows_manager = Application::get().get_windows_manager();
      ImGui::Checkbox("Fullscreen", &settings_manager.fullscreen);
      windows_manager.toggle_fullscreen(settings_manager.fullscreen);

      if (render_manager.get_framebuffers_count() == 0) {
        ImGui::BeginDisabled();
      }
      ImGui::Checkbox("Window inside Imgui", &settings_manager.imgui_window);
      if (render_manager.get_framebuffers_count() == 0) {
        ImGui::EndDisabled();
      }
      ImGui::SameLine();
      help_mark("Only works with a framebuffer");
      windows_manager.toggle_window_inside_imgui(settings_manager.imgui_window);
      if (render_manager.get_framebuffers_count() == 0) {
        ImGui::BeginDisabled();
      }
      ImGui::Checkbox("Keep ratio", &settings_manager.fit_to_window);
      if (render_manager.get_framebuffers_count() == 0) {
        ImGui::EndDisabled();
      }
      ImGui::SameLine();
      help_mark("Only works with a framebuffer");
      windows_manager.toggle_fit_to_window(settings_manager.fit_to_window);

      bool fullscreen = settings_manager.fullscreen;
      if (fullscreen) {
        ImGui::BeginDisabled();
      }
      ImGui::InputInt("Window width", &settings_manager.window_w);
      if (fullscreen) {
        ImGui::EndDisabled();
      }
      ImGui::SameLine();
      help_mark("Only works in windowed mode");
      if (fullscreen) {
        ImGui::BeginDisabled();
      }
      ImGui::InputInt("Window height", &settings_manager.window_h);
      if (fullscreen) {
        ImGui::EndDisabled();
      }
      ImGui::SameLine();
      help_mark("Only works in windowed mode");
      windows_manager.resize(settings_manager.window_w,
                             settings_manager.window_h);
      if (fullscreen) {
        ImGui::BeginDisabled();
      }
      ImGui::Checkbox("Resizable", &settings_manager.resizable);
      if (fullscreen) {
        ImGui::EndDisabled();
      }
      ImGui::SameLine();
      help_mark("Only works in windowed mode");
      windows_manager.toggle_resizable(settings_manager.resizable);

      if (not fullscreen) {
        ImGui::BeginDisabled();
      }
      ImGui::InputInt("Refresh rate", &settings_manager.refresh_rate);
      if (not fullscreen) {
        ImGui::EndDisabled();
      }
      ImGui::SameLine();
      help_mark("Only works in fullscreen");
      windows_manager.set_refresh_rate(settings_manager.refresh_rate);

      ImGui::InputInt("Depth bits", &settings_manager.depth_bits);
      ImGui::SameLine();
      help_mark("Requires restart");
      ImGui::Checkbox("vsync", &settings_manager.vsync);
      windows_manager.toggle_vsync(settings_manager.vsync);
      ImGui::InputInt("Primary monitor", &settings_manager.primary_monitor);
      windows_manager.set_window_monitor(settings_manager.primary_monitor);

      ImGui::SeparatorText("Info");
      ImGui::Text(std::format("OpenGL version {}.{}",
                              settings_manager.opengl_major,
                              settings_manager.opengl_minor)
                    .c_str());
      const auto& windowData = windows_manager.get_window_data();
      ImGui::Text(std::format("Window position x:{}, y:{}",
                              windowData.position_x, windowData.position_y)
                    .c_str());
      ImGui::Text(std::format("Mouse position x:{}, y:{}",
                              windowData.debug_mouse_x,
                              windowData.debug_mouse_y)
                    .c_str());

    } else if (selected_settings_manager_tabkey == "Debug") {
      ImGui::Checkbox("Game paused", &paused);
      app.pause(paused);
      ImGui::Checkbox("Debug enabled",
                      &settings_manager.enable_debug); // TODO use for something
      ImGui::Checkbox("Display FPS",
                      &settings_manager.display_fps); // TODO use for something
      ImGui::Checkbox("Display collision boxes",
                      &settings_manager.display_collision_boxes);
    } else if (selected_settings_manager_tabkey == "Logger") {
      ImGui::Checkbox("Enable engine logger",
                      &settings_manager.enable_engine_logger);
      LogManager::toggle_engine_logger(settings_manager.enable_engine_logger);
      if (ImGui::BeginCombo("Engine log level",
                            settings_manager.engine_log_level == 0   ? "trace"
                            : settings_manager.engine_log_level == 1 ? "debug"
                            : settings_manager.engine_log_level == 2 ? "info"
                            : settings_manager.engine_log_level == 3 ? "warning"
                            : settings_manager.engine_log_level == 4 ? "error"
                            : settings_manager.engine_log_level == 5
                              ? "critical"
                              : "unknown")) {
        for (uint32_t n = 0; n < settings_manager.log_levels.size(); n++) {
          bool is_selected = (settings_manager.engine_log_level == n);
          if (ImGui::Selectable(settings_manager.log_levels[n], is_selected)) {
            if (settings_manager.engine_log_level not_eq n) {
              settings_manager.engine_log_level = n;
              LogManager::set_engine_logger_level(n);
            }
          }
          if (is_selected) {
            ImGui::SetItemDefaultFocus();
          }
        }
        ImGui::EndCombo();
      }
      if (ImGui::BeginCombo(
            "Engine log flush level",
            settings_manager.engine_flush_level == 0   ? "trace"
            : settings_manager.engine_flush_level == 1 ? "debug"
            : settings_manager.engine_flush_level == 2 ? "info"
            : settings_manager.engine_flush_level == 3 ? "warning"
            : settings_manager.engine_flush_level == 4 ? "error"
            : settings_manager.engine_flush_level == 5 ? "critical"
                                                       : "unknown")) {
        for (uint32_t n = 0; n < settings_manager.log_levels.size(); n++) {
          bool is_selected = (settings_manager.engine_flush_level == n);
          if (ImGui::Selectable(settings_manager.log_levels[n], is_selected)) {
            if (settings_manager.engine_flush_level not_eq n) {
              settings_manager.engine_flush_level = n;
              LogManager::set_engine_logger_flush_level(n);
            }
          }
          if (is_selected) {
            ImGui::SetItemDefaultFocus();
          }
        }
        ImGui::EndCombo();
      }
      ImGui::Checkbox("Enable app logger", &settings_manager.enable_app_logger);
      LogManager::toggle_app_logger(settings_manager.enable_app_logger);
      if (ImGui::BeginCombo("App log level",
                            settings_manager.app_log_level == 0   ? "trace"
                            : settings_manager.app_log_level == 1 ? "debug"
                            : settings_manager.app_log_level == 2 ? "info"
                            : settings_manager.app_log_level == 3 ? "warning"
                            : settings_manager.app_log_level == 4 ? "error"
                            : settings_manager.app_log_level == 5
                              ? "critical"
                              : "unknown")) {
        for (uint32_t n = 0; n < settings_manager.log_levels.size(); n++) {
          bool is_selected = (settings_manager.app_log_level == n);
          if (ImGui::Selectable(settings_manager.log_levels[n], is_selected)) {
            if (settings_manager.app_log_level not_eq n) {
              settings_manager.app_log_level = n;
              LogManager::set_app_logger_level(n);
            }
          }
          if (is_selected) {
            ImGui::SetItemDefaultFocus();
          }
        }
        ImGui::EndCombo();
      }
      if (ImGui::BeginCombo("App log flush level",
                            settings_manager.app_flush_level == 0   ? "trace"
                            : settings_manager.app_flush_level == 1 ? "debug"
                            : settings_manager.app_flush_level == 2 ? "info"
                            : settings_manager.app_flush_level == 3 ? "warning"
                            : settings_manager.app_flush_level == 4 ? "error"
                            : settings_manager.app_flush_level == 5
                              ? "critical"
                              : "unknown")) {
        for (uint32_t n = 0; n < settings_manager.log_levels.size(); n++) {
          bool is_selected = (settings_manager.app_flush_level == n);
          if (ImGui::Selectable(settings_manager.log_levels[n], is_selected)) {
            if (settings_manager.app_flush_level not_eq n) {
              settings_manager.app_flush_level = n;
              LogManager::set_app_logger_flush_level(n);
            }
          }
          if (is_selected) {
            ImGui::SetItemDefaultFocus();
          }
        }
        ImGui::EndCombo();
      }
      ImGui::Checkbox("Enable engine backtrace logger",
                      &settings_manager.enable_engine_backtrace_logger);
      LogManager::toggle_engine_backtrace_logger(
        settings_manager.enable_engine_backtrace_logger);
      ImGui::Checkbox("Enable app backtrace logger",
                      &settings_manager.enable_app_backtrace_logger);
      LogManager::toggle_app_backtrace_logger(
        settings_manager.enable_app_backtrace_logger);
      if (ImGui::Button("Clear all backtrace logger")) {
        LogManager::clear_all_backtrace_logger();
      }
      if (ImGui::Button("Clear engine backtrace logger")) {
        LogManager::clear_engine_backtrace_logger();
      }
      if (ImGui::Button("Clear app backtrace logger")) {
        LogManager::clear_app_backtrace_logger();
      }
      if (ImGui::Button("Dump backtrace")) {
        LogManager::dump_backtrace();
      }
    } else if (selected_settings_manager_tabkey == "Render") {
      ImGui::ColorEdit4("Clear color", settings_manager.clear_color.data());
      ImGui::SliderFloat("Clear depth", &settings_manager.clear_depth, 0.f,
                         1.f);
    } else if (selected_settings_manager_tabkey == "Scene") {
      ImGui::Text("Name: %s", settings_manager.active_scene.c_str());
      ImGui::Text("Path: %s ", settings_manager.active_scene_path.c_str());
      ImGui::Checkbox("Reload scene", &settings_manager.reload_scene);
    }
  }

  ImGui::Columns(1);
}
}