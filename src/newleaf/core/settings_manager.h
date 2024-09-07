#pragma once

#include <nlohmann/json.hpp>

using json = nlohmann::json;

#include <array>
#include <string>
#include <vector>

namespace nl {

struct SettingsManager {
    std::string app_name = "newleaf";
    std::string root = "..";
    std::string logfile_path = "logs/newleaf.log";
    std::string backtrace_logfile_path = "logs/backtrace.log";

    std::string window_icon_path = "";
    int window_w = 1280;
    int window_h = 720;
    int depth_bits = 24;
    int refresh_rate = 60;   // only used in fullscreen mode
    bool fullscreen = false; // true: fullscreen, false: windowed
    int primary_monitor = 0; // 0: primary, 1: secondary
    bool vsync = true;
    bool resizable = true;
    uint32_t opengl_major = 4; // only 4.6 is supported
    uint32_t opengl_minor = 6;
    bool imgui_window = true; // it needs an opengl fbo to work
    bool fit_to_window = true;

    std::string cursor_icon_path = "assets/textures/cursor.png";
    uint32_t cursor_mode = 2; // 0: normal, 1: hidden, 2: disabled

    bool enable_debug = true; // TODO use for something
    bool display_fps = false; // TODO implement with enable_debug maybe?
    bool display_collision_boxes = false;

    bool enable_engine_logger = true;
    bool enable_app_logger = true;
    // 0: trace, 1: debug, 2: info, 3: warning, 4: error, 5: critical
    uint32_t engine_log_level{};
    uint32_t engine_flush_level{};
    uint32_t app_log_level{};
    uint32_t app_flush_level{};
#ifdef DEBUG
    bool enable_engine_backtrace_logger = true;
    bool enable_app_backtrace_logger = true;
#else
    bool enable_engine_backtrace_logger = false;
    bool enable_app_backtrace_logger = false;
#endif

    std::array<float, 4> clear_color = {0.45f, 0.55f, 0.6f, 1.f};
    float clear_depth = 1.f;

    std::string active_scene{""};
    std::string active_scene_path{""};
    bool reload_scene = false;
    bool reload_prototypes = false;

    std::vector<const char*> cursor_modes{"normal", "hidden", "disabled"};
    std::vector<const char*> log_levels{"trace",   "debug", "info",
                                        "warning", "error", "critical"};
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
  SettingsManager, app_name, root, logfile_path, backtrace_logfile_path,
  window_icon_path, window_w, window_h, depth_bits, refresh_rate, fullscreen,
  primary_monitor, vsync, resizable, imgui_window, fit_to_window,
  cursor_icon_path, cursor_mode, enable_debug, display_fps,
  enable_engine_logger, enable_app_logger, engine_log_level, app_log_level,
  engine_flush_level, app_flush_level, enable_engine_backtrace_logger,
  enable_app_backtrace_logger, clear_color, clear_depth, active_scene,
  active_scene_path, reload_prototypes, display_collision_boxes);
}