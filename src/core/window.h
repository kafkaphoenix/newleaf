#pragma once

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#define GLM_FORCE_CTOR_INIT
#include <glm/glm.hpp>

#include "events/event.h"
#include "pch.h"
#include "renderer/openGLContext.h"

namespace potatoengine {

enum class CursorMode {
  Normal = GLFW_CURSOR_NORMAL,
  Hidden = GLFW_CURSOR_HIDDEN,
  Disabled = GLFW_CURSOR_DISABLED
};
struct WindowProperties {
    std::string name{};
    std::string windowIconPath{};
    glm::vec2 windowSize{};
    int depthBits{};
    int refreshRate{};
    bool fullscreen{};
    int primaryMonitor{};
    bool vSync{};
    bool resizable{};
    int openglMajorVersion{};
    int openglMinorVersion{};
    std::string cursorIconPath{};
    CursorMode cursorMode{};
    bool windowInsideImgui{};
    bool fitToWindow{};
};

static int s_GLFWWindowCount = 0;

class Window {
  public:
    using EventCallbackFn = std::function<void(Event&)>;

    Window(WindowProperties&& properties);
    ~Window();

    void shutdown();
    void onUpdate();
    void onEvent();

    GLFWwindow* getNativeWindow() const noexcept { return m_window; }
    int getWidth() const { return m_data.size.x; }
    int getHeight() const { return m_data.size.y; }
    bool isVSync() const { return m_data.vSync; }
    bool isFullscreen() const { return m_data.fullscreen; }
    bool isMaximized() const { return m_data.maximized; }
    bool isMinimized() const { return m_data.minimized; }
    bool isWireframe() const { return m_data.wireframe; }
    bool isWindowInsideImgui() const { return m_data.windowInsideImgui; }
    bool fitToWindow() const { return m_data.fitToWindow; }

    void setEventCallback(const EventCallbackFn& cb) {
      m_data.eventCallback = cb;
    }
    void setWindowTitle(const std::string& title);
    void setWindowIcon(const std::string& path);
    void restoreWindowIcon();
    void setWindowMonitor(int monitor);
    void toggleVSync(bool enabled);
    void setCursorIcon(const std::string& path);
    void setCursorMode(CursorMode mode);
    void restoreCursor();
    void setResizable(bool resizable);
    void setRefreshRate(int refreshRate);
    void setSize(glm::vec2&& size);
    void setPosition(int x, int y);
    void minimize(bool minimize);
    void maximize(bool maximize);
    void setFocus(bool focused);
    void setFullscreen(bool fullscreen);
    void setVisible(bool visible);
    void updateCameraPosition(bool update) {
      m_data.updateCameraPosition = update;
    }
    void setLastMousePosition(float x, float y) {
      m_data.lastX = x;
      m_data.lastY = y;
    }
    void toggleWindowInsideImgui(bool windowInsideImgui);
    void toggleWireframe(bool wireframe);
    void toggleFitToWindow(bool fitToWindow);

    static std::unique_ptr<Window> Create(WindowProperties&& properties);

  private:
    GLFWwindow* m_window{}; // TODO: this class should be a window manager, and
                            // this should be a vector of windows
    std::unique_ptr<OpenGLContext> m_context;

    struct WindowData {
        std::string name{};
        glm::vec2 size{};
        std::string windowIconPath{};
        int primaryMonitor{};
        int refreshRate{};
        bool maximized{};
        bool minimized{};
        float lastX{};
        float lastY{};
        bool firstMouse = true;
        bool vSync{};
        bool fullscreen{};
        CursorMode cursorMode = CursorMode::Disabled;
        GLFWcursor* cursor;
        bool updateCameraPosition = true;
        bool wireframe{};
        bool windowInsideImgui{};
        bool fitToWindow{};

        EventCallbackFn eventCallback;
    };

    WindowData m_data{};
};

}