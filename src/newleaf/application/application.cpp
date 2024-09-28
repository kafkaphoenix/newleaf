#include "application.h"

#include "../assets/assets_manager.h"
#include "../events/event.h"
#include "../graphics/render_manager.h"
#include "../logging/log_manager.h"
#include "../scene/scene_manager.h"
#include "../settings/settings_manager.h"
#include "../state/imgui_layer.h"
#include "../state/state.h"
#include "../state/states_manager.h"
#include "../window/windows_manager.h"

namespace nl {

Application::Application(std::unique_ptr<SettingsManager>&& s, CLArgs&& args)
  : m_clargs(std::move(args)) {
  m_instance = this;
  m_settings_manager = std::move(s);

  m_name = m_settings_manager->app_name;
  std::filesystem::current_path(m_settings_manager->root);
  m_states_manager = StatesManager::create();
  m_assets_manager = AssetsManager::create();

  m_windows_manager = WindowsManager::create(*m_settings_manager);
  m_windows_manager->set_event_callback(BIND_EVENT(on_event));

  m_render_manager = RenderManager::create();
  m_render_manager->init();
  m_scene_manager = SceneManager::create();
  m_imgui_layer = std::make_unique<ImGuiLayer>();
  m_imgui_layer->on_attach();
}

Application::~Application() {
  ENGINE_WARN("deleting application");
  m_render_manager->shutdown();
  m_imgui_layer->on_detach();
}

void Application::on_event(Event& e) {
  m_states_manager->get_current_state().on_event(e);
}

void Application::pause(bool pause) { m_paused = pause; }

void Application::run() {
  while (m_running) {
    float currentFrame = (float)glfwGetTime();
    Time ts = currentFrame - m_lastFrame;
    m_lastFrame = currentFrame;
    m_accumulator += ts;

    if (not m_minimized) [[likely]] {
      while (m_accumulator > ts) {
        // to be able to render inside an imgui window
        m_imgui_layer->begin();
        auto& current_state = m_states_manager->get_current_state();
        current_state.on_update(ts);
        m_scene_manager->on_update(ts);

        m_imgui_layer->on_imgui_update();
        current_state.on_imgui_update();
        m_imgui_layer->end();

        m_accumulator -= ts;
        if (m_accumulator < 0) {
          m_accumulator = 0;
        }
      }
    }

    m_windows_manager->on_update();
  }
}
}