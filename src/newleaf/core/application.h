#pragma once

#include "../assets/assets_manager.h"
#include "../core/settings_manager.h"
#include "../core/state.h"
#include "../core/states_manager.h"
#include "../core/windows_manager.h"
#include "../events/event.h"
#include "../imgui/imgui_layer.h"
#include "../pch.h"
#include "../render/render_manager.h"
#include "../scene/scene_manager.h"

int main(int argc, char** argv);

namespace nl {

struct CLArgs {
    std::span<const char*> args;

    const char* operator[](int index) const { return args[index]; }
};

class Application {
  public:
    Application(std::unique_ptr<SettingsManager>&& settings_manager,
                CLArgs&& args);
    virtual ~Application();

    void on_event(events::Event& e);

    const std::unique_ptr<WindowsManager>& get_window_manager() const {
      return m_windows_manager;
    }
    const std::unique_ptr<SceneManager>& get_scene_manager() const {
      return m_scene_manager;
    }
    const std::unique_ptr<assets::AssetsManager>& get_assets_manager() const {
      return m_assets_manager;
    }
    const std::unique_ptr<RenderManager>& get_render_manager() const {
      return m_render_manager;
    }
    const std::unique_ptr<SettingsManager>& get_settings_manager() const {
      return m_settings_manager;
    }
    const std::unique_ptr<StatesManager>& get_states_manager() const {
      return m_states_manager;
    }

    void close() { m_running = false; }
    void minimize(bool minimize) { m_minimized = minimize; }
    void pause(bool pause);
    void restore_pause(bool restore) { m_restore_pause = restore; }
    void debug(bool debugging) { m_debugging = debugging; }

    bool is_minimized() const { return m_minimized; }
    bool is_paused() const { return m_paused; }
    bool should_restore_pause() const { return m_restore_pause; }
    bool is_debugging() const { return m_debugging; }

    static Application& Get() { return *s_instance; }

  protected:
    std::unique_ptr<SceneManager> m_scene_manager;
    std::unique_ptr<assets::AssetsManager> m_assets_manager;
    std::unique_ptr<RenderManager> m_render_manager;
    std::unique_ptr<SettingsManager> m_settings_manager;
    std::unique_ptr<StatesManager> m_states_manager;
    std::unique_ptr<WindowsManager> m_windows_manager;
    std::unique_ptr<ImGuiLayer> m_imgui_layer;

  private:
    void run();

    std::string m_name;
    bool m_running{true};
    bool m_minimized{};
    bool m_paused{};
    bool m_restore_pause{};
    bool m_debugging{};
    float m_lastFrame{};
    float m_accumulator{};

    CLArgs m_clargs;

    inline static Application* s_instance;
    friend int ::main(int argc, char** argv);
};

// to be defined in user app
Application* Create(CLArgs&& args);
}