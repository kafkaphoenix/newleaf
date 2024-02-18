#pragma once

#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include "core/settings_manager.h"
#include "events/event.h"
#include "pch.h"
#include "render/opengl_context.h"

namespace potatoengine {

using EventCallbackFn = std::function<void(events::Event&)>;

enum class cursor_mode { Normal = 0, Hidden, Disabled };

struct WindowData {
    int position_x;
    int position_y;
    float mouse_x;
    float mouse_y;
    float debug_mouse_x;
    float debug_mouse_y;
    bool is_first_mouse{true};
    bool should_update_camera_position{true};
    bool minimized{};
    bool maximized{};
    bool focused{};
    bool visible{};
    bool wireframe{};
    EventCallbackFn event_callback;

    // settings
    std::string window_title;
    std::string window_icon_path;
    int width;
    int height;
    int primary_monitor;
    bool vsync{};
    std::string cursor_icon_path;
    GLFWcursor* cursor;
    cursor_mode cursor_mode;
    bool resizable{};
    int refresh_rate;
    bool fullscreen{};
    bool is_imgui_window{};
    bool fit_to_window{};
};

static uint32_t s_glfw_window_count{};

class WindowsManager {
  public:
    WindowsManager(const std::unique_ptr<SettingsManager>& settings_manager);
    ~WindowsManager();

    void shutdown();
    void on_update();
    void trigger_event(events::Event& e);

    GLFWwindow* get_native_window() const { return m_window; }
    const WindowData& get_window_data() const { return m_data; }

    void set_position(int x, int y);
    void set_last_mouse_position(float x, float y);
    void update_camera_position(bool enable);
    void minimize(bool minimize);
    void maximize(bool maximize);
    void toggle_focus(bool focused);
    void toggle_visibility(bool visible);
    void toggle_wireframe(bool wireframe);
    void set_event_callback(EventCallbackFn&& cb);

    void set_window_title(std::string title);
    void set_window_icon(std::string path);
    void restore_window_icon();
    void set_cursor_icon(std::string path);
    void set_cursor_mode(cursor_mode mode, bool update);
    void restore_cursor();
    void resize(int width, int height);
    void toggle_resizable(bool resizable);
    void set_refresh_rate(int refresh_rate);
    void toggle_vsync(bool enabled);
    void set_window_monitor(int monitor);
    void toggle_fullscreen(bool fullscreen);
    void toggle_window_inside_imgui(bool is_imgui_window);
    void toggle_fit_to_window(bool fit_to_window);

    static std::unique_ptr<WindowsManager>
    Create(const std::unique_ptr<SettingsManager>& settings_manager);

  private:
    GLFWwindow* m_window{}; // TODO: this class should be a window manager, and
                            // this should be a vector of windows
    std::unique_ptr<OpenGLContext> m_context;
    WindowData m_data{};
};

}