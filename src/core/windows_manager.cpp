#include "core/windows_manager.h"

#include <stb_image.h>

#include "core/application.h"
#include "events/key_event.h"
#include "events/mouse_event.h"
#include "events/window_event.h"

namespace potatoengine {

WindowsManager::WindowsManager(
  const std::unique_ptr<SettingsManager>& settings_manager) {
  m_data.window_title = settings_manager->app_name;
  m_data.width = settings_manager->window_w;
  m_data.height = settings_manager->window_h;
  m_data.primary_monitor = settings_manager->primary_monitor;
  m_data.mouse_x = m_data.width / 2.0f;  // center of the screen
  m_data.mouse_y = m_data.height / 2.0f; // center of the screen
  m_data.is_imgui_window = settings_manager->is_imgui_window;
  m_data.fit_to_window = settings_manager->fit_to_window;
  ENGINE_TRACE("Creating window {} with resolution {}x{}...",
               m_data.window_title, m_data.width, m_data.height);
  if (s_glfw_window_count == 0) {
    ENGINE_ASSERT(glfwInit(), "Failed to initialize GLFW!");
    glfwSetErrorCallback([](int error, const char* description) {
      ENGINE_ASSERT(false, "GLFW Error! {0}: {1}", error, description);
    });
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, settings_manager->opengl_major);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, settings_manager->opengl_minor);
  glfwWindowHint(GLFW_DEPTH_BITS, settings_manager->depth_bits);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_RESIZABLE, settings_manager->resizable);
  m_data.resizable = settings_manager->resizable;
  glfwWindowHint(GLFW_REFRESH_RATE, settings_manager->refresh_rate);
  m_data.refresh_rate = settings_manager->refresh_rate;

  ENGINE_TRACE("Loading OpengGL version {}.{}", settings_manager->opengl_major,
               settings_manager->opengl_minor);

  int monitorCount;
  GLFWmonitor** monitors = glfwGetMonitors(&monitorCount);
  if (settings_manager->primary_monitor < 0 or
      settings_manager->primary_monitor >= monitorCount) {
    ENGINE_ERROR("Invalid monitor index!");
    return;
  }
  const GLFWvidmode* mode =
    glfwGetVideoMode(monitors[settings_manager->primary_monitor]);
  int xpos = (mode->width - m_data.width) / 2;
  int ypos = (mode->height - m_data.height) / 2;
  if (settings_manager->fullscreen) {
    m_data.fullscreen = true;
    m_window =
      glfwCreateWindow(mode->width, mode->height, m_data.window_title.c_str(),
                       monitors[settings_manager->primary_monitor], nullptr);
    // center of the screen to avoid 0 0 when windowed first time
    m_data.position_x = xpos;
    m_data.position_y = ypos;
  } else {
    m_data.fullscreen = false;
    m_window = glfwCreateWindow(m_data.width, m_data.height,
                                m_data.window_title.c_str(), nullptr, nullptr);
    set_position(xpos, ypos);
  }

  ++s_glfw_window_count;

  m_context = OpenGLContext::Create(m_window);
  m_context->init(); // make context current window
  if (m_data.fullscreen) {
    glViewport(0, 0, mode->width, mode->height);
  } else {
    glViewport(0, 0, m_data.width, m_data.height);
  }

  set_window_icon(settings_manager->window_icon_path);
  set_cursor_mode(static_cast<cursor_mode>(settings_manager->cursor_mode),
                  true);
  set_cursor_icon(settings_manager->cursor_icon_path);
  toggle_vsync(settings_manager->vsync);
  glfwSetWindowUserPointer(m_window, &m_data);

  glfwSetFramebufferSizeCallback(
    m_window, [](GLFWwindow* window, int width, int height) {
      WindowData& data =
        *std::bit_cast<WindowData*>(glfwGetWindowUserPointer(window));

      if (not data.fullscreen) {
        data.width = width;
        data.height = height;
      }

      // for fullscreen it will update the resolution
      events::WindowResizeEvent event(width, height);
      data.event_callback(event);
    });

  glfwSetWindowPosCallback(
    m_window, [](GLFWwindow* window, int xpos, int ypos) {
      WindowData& data =
        *std::bit_cast<WindowData*>(glfwGetWindowUserPointer(window));

      if (not data.fullscreen) {
        data.position_x = xpos;
        data.position_y = ypos;
        events::WindowMovedEvent event(xpos, ypos);
        data.event_callback(event);
      }
    });

  glfwSetWindowCloseCallback(m_window, [](GLFWwindow* window) {
    WindowData& data =
      *std::bit_cast<WindowData*>(glfwGetWindowUserPointer(window));
    events::WindowCloseEvent event;
    data.event_callback(event);
  });

  glfwSetKeyCallback(
    m_window, [](GLFWwindow* window, int key, int, int action, int) {
      WindowData& data =
        *std::bit_cast<WindowData*>(glfwGetWindowUserPointer(window));

      switch (action) {
      case GLFW_PRESS: {
        events::KeyPressedEvent event(key, false);
        data.event_callback(event);
        break;
      }
      case GLFW_RELEASE: {
        events::KeyReleasedEvent event(key);
        data.event_callback(event);
        break;
      }
      case GLFW_REPEAT: {
        events::KeyPressedEvent event(key, true);
        data.event_callback(event);
        break;
      }
      }
    });

  glfwSetCharCallback(m_window, [](GLFWwindow* window, uint32_t keycode) {
    WindowData& data =
      *std::bit_cast<WindowData*>(glfwGetWindowUserPointer(window));

    events::KeyTypedEvent event(keycode);
    data.event_callback(event);
  });

  glfwSetMouseButtonCallback(
    m_window, [](GLFWwindow* window, int button, int action, int) {
      WindowData& data =
        *std::bit_cast<WindowData*>(glfwGetWindowUserPointer(window));

      switch (action) {
      case GLFW_PRESS: {
        events::MouseButtonPressedEvent event(button);
        data.event_callback(event);
        break;
      }
      case GLFW_RELEASE: {
        events::MouseButtonReleasedEvent event(button);
        data.event_callback(event);
        break;
      }
      }
    });

  glfwSetCursorPosCallback(
    m_window, [](GLFWwindow* window, double xpos, double ypos) {
      WindowData& data =
        *std::bit_cast<WindowData*>(glfwGetWindowUserPointer(window));

      if (data.is_first_mouse) {
        data.debug_mouse_x = (float)xpos;
        data.debug_mouse_y = (float)ypos;
        data.is_first_mouse = false;
      }

      float xoffset = (float)xpos - data.debug_mouse_x;
      float yoffset =
        data.debug_mouse_y -
        (float)ypos; // reversed since y-coordinates go from bottom to top

      data.debug_mouse_x = (float)xpos;
      data.debug_mouse_y = (float)ypos;

      if (not data.should_update_camera_position) {
        return;
      }

      data.mouse_x = data.debug_mouse_x;
      data.mouse_y = data.debug_mouse_y;

      events::MouseMovedEvent event(xoffset, yoffset);
      data.event_callback(event);
    });

  glfwSetScrollCallback(
    m_window, [](GLFWwindow* window, double xoffset, double yoffset) {
      WindowData& data =
        *std::bit_cast<WindowData*>(glfwGetWindowUserPointer(window));

      if (not data.should_update_camera_position) {
        return;
      }

      events::MouseScrolledEvent event((float)xoffset, (float)yoffset);
      data.event_callback(event);
    });

  glfwSetWindowIconifyCallback(m_window, [](GLFWwindow* window, int minimized) {
    WindowData& data =
      *std::bit_cast<WindowData*>(glfwGetWindowUserPointer(window));

    if (minimized == GLFW_TRUE) {
      data.minimized = true;
      events::WindowMinimizedEvent event;
      data.event_callback(event);
    } else {
      data.minimized = false;
      events::WindowRestoredEvent event;
      data.event_callback(event);
    }
  });

  glfwSetWindowMaximizeCallback(
    m_window, [](GLFWwindow* window, int maximized) {
      WindowData& data =
        *std::bit_cast<WindowData*>(glfwGetWindowUserPointer(window));

      if (maximized == GLFW_TRUE) {
        data.maximized = true;
        events::WindowMaximizedEvent event;
        data.event_callback(event);
      } else {
        data.maximized = false;
        events::WindowRestoredEvent event;
        data.event_callback(event);
      }
    });

  glfwSetWindowFocusCallback(m_window, [](GLFWwindow* window, int focused) {
    WindowData& data =
      *std::bit_cast<WindowData*>(glfwGetWindowUserPointer(window));

    if (focused == GLFW_TRUE) {
      data.focused = true;
      events::WindowFocusEvent event;
      data.event_callback(event);
    } else {
      data.focused = false;
      events::WindowLostFocusEvent event;
      data.event_callback(event);
    }
  });
}

WindowsManager::~WindowsManager() {
  ENGINE_WARN("Deleting window");
  shutdown();
}

void WindowsManager::shutdown() {
  glfwDestroyWindow(m_window);
  --s_glfw_window_count;

  if (s_glfw_window_count == 0) {
    ENGINE_WARN("No more windows! Terminating GLFW");
    glfwTerminate();
  }
}

void WindowsManager::on_update() {
  m_context->swap_buffers();
  glfwPollEvents();
}

void WindowsManager::trigger_event(events::Event& e) {
  m_data.event_callback(e);
}

void WindowsManager::set_position(int x, int y) {
  if (Application::Get().get_settings_manager()->fullscreen) {
    ENGINE_ERROR("Cannot set position of fullscreen window!");
    return;
  }

  if (x not_eq m_data.position_x or y not_eq m_data.position_y) {
    glfwSetWindowPos(m_window, x, y);
    m_data.position_x = x;
    m_data.position_y = y;
  }
}

void WindowsManager::set_last_mouse_position(float x, float y) {
  m_data.mouse_x = x;
  m_data.mouse_y = y;
}

void WindowsManager::update_camera_position(bool enable) {
  m_data.should_update_camera_position = enable;
}

void WindowsManager::minimize(bool minimize) {
  if (minimize and not m_data.minimized) {
    glfwIconifyWindow(m_window);
  } else if (not minimize and m_data.minimized) {
    glfwRestoreWindow(m_window);
  }
}

void WindowsManager::maximize(bool maximize) {
  if (Application::Get().get_settings_manager()->fullscreen) {
    ENGINE_ERROR("Cannot maximize fullscreen window!");
    return;
  }

  if (maximize and not m_data.maximized) {
    glfwMaximizeWindow(m_window);
  } else if (not maximize and m_data.maximized) {
    glfwRestoreWindow(m_window);
  }
}

void WindowsManager::toggle_focus(bool focused) {
  if (Application::Get().get_settings_manager()->fullscreen) {
    ENGINE_ERROR("Cannot set focus of fullscreen window!");
    return;
  }

  if (not m_data.visible) {
    ENGINE_ERROR("Cannot set focus of invisible window!");
    return;
  }

  if (m_data.minimized) {
    ENGINE_ERROR("Cannot set focus of minimized window!");
    return;
  }

  if (focused and not m_data.focused) {
    glfwFocusWindow(m_window);
  } else if (not focused and m_data.focused) {
    glfwFocusWindow(nullptr);
  }
}

void WindowsManager::toggle_visibility(bool visible) {
  if (visible and m_data.visible) {
    ENGINE_ERROR("Cannot show visible window!");
    return;
  }

  if (not visible and not m_data.visible) {
    ENGINE_ERROR("Cannot hide invisible window!");
    return;
  }

  if (visible and not m_data.visible) {
    glfwShowWindow(m_window);
    m_data.visible = visible;
  } else if (not visible and m_data.visible) {
    glfwHideWindow(m_window);
    m_data.visible = visible;
  }
}

void WindowsManager::toggle_wireframe(bool wireframe) {
  if (wireframe not_eq m_data.wireframe) {
    m_data.wireframe = wireframe;
  }
}

void WindowsManager::set_event_callback(EventCallbackFn&& cb) {
  m_data.event_callback = std::move(cb);
}

void WindowsManager::set_window_title(std::string title) {
  if (title.empty()) {
    ENGINE_ERROR("WindowsManager title is empty!");
    return;
  }

  if (title not_eq m_data.window_title) {
    glfwSetWindowTitle(m_window, title.c_str());
    m_data.window_title = title;
    Application::Get().get_settings_manager()->app_name = title;
  }
}

void WindowsManager::set_window_icon(std::string path) {
  if (path.empty()) {
    ENGINE_ERROR("WindowsManager icon path is empty!");
  }

  if (not std::filesystem::exists(path)) {
    ENGINE_ERROR("WindowsManager icon path {} does not exist!", path);
    return;
  }

  if (path not_eq m_data.window_icon_path) {
    GLFWimage images[1];
    images[0].pixels =
      stbi_load(path.c_str(), &images[0].width, &images[0].height, 0, 4);
    glfwSetWindowIcon(m_window, 1, images);
    stbi_image_free(images[0].pixels);
    m_data.window_icon_path = path;
    Application::Get().get_settings_manager()->window_icon_path = path;
  }
}

void WindowsManager::restore_window_icon() {
  if (m_data.window_icon_path.empty()) {
    ENGINE_ERROR("WindowsManager icon path is empty!");
    return;
  }

  set_window_icon(m_data.window_icon_path);
}

void WindowsManager::set_cursor_icon(std::string path) {
  if (path.empty()) {
    ENGINE_ERROR("Cursor icon path is empty!");
    return;
  }

  if (not std::filesystem::exists(path)) {
    ENGINE_ERROR("Cursor icon path {} does not exist!", path);
    return;
  }

  if (path not_eq m_data.cursor_icon_path) {
    if (m_data.cursor not_eq nullptr) {
      glfwDestroyCursor(m_data.cursor);
    }

    GLFWimage images[1];
    images[0].pixels =
      stbi_load(path.c_str(), &images[0].width, &images[0].height, 0, 4);
    m_data.cursor = glfwCreateCursor(&images[0], 0, 0);
    glfwSetCursor(m_window, m_data.cursor);
    stbi_image_free(images[0].pixels);
    m_data.cursor_icon_path = path;
    Application::Get().get_settings_manager()->cursor_icon_path = path;
  }
}

void WindowsManager::set_cursor_mode(cursor_mode cursor_mode, bool update) {
  if (cursor_mode not_eq m_data.cursor_mode) {
    int mode = static_cast<int>(cursor_mode);
    if (mode == 0) {
      glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    } else if (mode == 1) {
      glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    } else if (mode == 2) {
      glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    } else {
      ENGINE_ERROR("Invalid cursor mode!");
      return;
    }
    if (update) {
      m_data.cursor_mode = cursor_mode;
      Application::Get().get_settings_manager()->cursor_mode = mode;
    }
  }
}

void WindowsManager::restore_cursor() {
  int mode = static_cast<int>(m_data.cursor_mode);
  if (mode == 0) {
    glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
  } else if (mode == 1) {
    glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
  } else if (mode == 2) {
    glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  } else {
    ENGINE_ERROR("Invalid cursor mode!");
    return;
  }

  if (m_data.cursor) { // restore cursor icon
    glfwSetCursor(m_window, m_data.cursor);
  }
}

void WindowsManager::resize(int width, int height) {
  // for fullscreen windows it updates the resolution
  // m_data is updated in the callback
  // and settings in the event
  if (width < 0 or height < 0) {
    ENGINE_ERROR("Invalid window size!");
    return;
  }
  if (width not_eq m_data.width or height not_eq m_data.height) {
    glfwSetWindowSize(m_window, width, height);
  }
}

void WindowsManager::toggle_resizable(bool resizable) {
  if (resizable not_eq m_data.resizable) {
    if (m_data.fullscreen) {
      ENGINE_ERROR("Cannot set resizable of fullscreen window!");
      return;
    }

    glfwSetWindowAttrib(m_window, GLFW_RESIZABLE, resizable);
    m_data.resizable = resizable;
    Application::Get().get_settings_manager()->resizable = resizable;
  }
}

void WindowsManager::set_refresh_rate(int refresh_rate) {
  if (refresh_rate not_eq m_data.refresh_rate) {
    if (not m_data.fullscreen) {
      ENGINE_ERROR("Cannot set refresh rate of windowed window!");
      return;
    }

    if (refresh_rate < 0) {
      ENGINE_ERROR("Invalid refresh rate!");
      return;
    }

    const auto& settings_manager = Application::Get().get_settings_manager();
    int monitorCount;
    GLFWmonitor* monitor =
      glfwGetMonitors(&monitorCount)[settings_manager->primary_monitor];
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    glfwSetWindowMonitor(m_window, monitor, GLFW_DONT_CARE, GLFW_DONT_CARE,
                         mode->width, mode->height, refresh_rate);
    m_data.refresh_rate = refresh_rate;
    settings_manager->refresh_rate = refresh_rate;
    if (m_data.vsync) {
      glfwSwapInterval(1); // https://github.com/glfw/glfw/issues/1072
    }
  }
}

void WindowsManager::toggle_vsync(bool enabled) {
  if (enabled not_eq m_data.vsync) {
    glfwSwapInterval(enabled ? 1 : 0);
    m_data.vsync = enabled;
    Application::Get().get_settings_manager()->vsync = enabled;
  }
}

void WindowsManager::set_window_monitor(int monitor) {
  if (monitor not_eq m_data.primary_monitor) {
    int monitorCount;
    GLFWmonitor** monitors = glfwGetMonitors(&monitorCount);
    if (monitor < 0 or monitor >= monitorCount) {
      ENGINE_ERROR("Invalid monitor index!");
      return;
    }

    const auto& settings_manager = Application::Get().get_settings_manager();
    int xpos = m_data.fullscreen ? GLFW_DONT_CARE : m_data.position_x;
    int ypos = m_data.fullscreen ? GLFW_DONT_CARE : m_data.position_y;
    int refresh_rate =
      m_data.fullscreen ? settings_manager->refresh_rate : GLFW_DONT_CARE;

    glfwSetWindowMonitor(m_window, monitors[monitor], xpos, ypos, m_data.width,
                         m_data.height, refresh_rate);
    m_data.primary_monitor = monitor;
    settings_manager->primary_monitor = monitor;
  }
}

void WindowsManager::toggle_fullscreen(bool fullscreen) {
  if (fullscreen and not m_data.fullscreen) {
    const auto& settings_manager = Application::Get().get_settings_manager();
    int monitorCount;
    GLFWmonitor* monitor =
      (glfwGetMonitors(&monitorCount))[settings_manager->primary_monitor];
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    m_data.fullscreen = fullscreen;
    settings_manager->fullscreen = fullscreen;
    glfwSetWindowMonitor(m_window, monitor, GLFW_DONT_CARE, GLFW_DONT_CARE,
                         mode->width, mode->height,
                         settings_manager->refresh_rate);
    if (m_data.vsync) {
      glfwSwapInterval(1); // https://github.com/glfw/glfw/issues/1072
    }
  } else if (not fullscreen and m_data.fullscreen) {
    m_data.fullscreen = fullscreen;
    Application::Get().get_settings_manager()->fullscreen = fullscreen;
    glfwSetWindowMonitor(m_window, nullptr, m_data.position_x,
                         m_data.position_y, m_data.width, m_data.height,
                         GLFW_DONT_CARE);
    restore_window_icon();
    if (m_data.vsync) {
      glfwSwapInterval(1); // https://github.com/glfw/glfw/issues/1072
    }
  }
}

void WindowsManager::toggle_window_inside_imgui(bool is_imgui_window) {
  if (is_imgui_window not_eq m_data.is_imgui_window) {
    m_data.is_imgui_window = is_imgui_window;
    Application::Get().get_settings_manager()->is_imgui_window =
      is_imgui_window;
  }
}

void WindowsManager::toggle_fit_to_window(bool fit_to_window) {
  if (fit_to_window not_eq m_data.fit_to_window) {
    m_data.fit_to_window = fit_to_window;
    Application::Get().get_settings_manager()->fit_to_window = fit_to_window;
  }
}

std::unique_ptr<WindowsManager> WindowsManager::Create(
  const std::unique_ptr<SettingsManager>& settings_manager) {
  return std::make_unique<WindowsManager>(settings_manager);
}
}