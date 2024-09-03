#include "../core/application.h"

#include "../core/time.h"
#include "../imgui/imgui_layer.h"

namespace nl {

Application::Application(std::unique_ptr<SettingsManager>&& s, CLArgs&& args)
  : m_clargs(std::move(args)) {
  s_instance = this;
  m_settings_manager = std::move(s);

  m_name = m_settings_manager->app_name;
  std::filesystem::current_path(m_settings_manager->root);
  m_states_manager = StatesManager::Create();
  m_assets_manager = assets::AssetsManager::Create();

  m_windows_manager = WindowsManager::Create(m_settings_manager);
  m_windows_manager->set_event_callback(BIND_EVENT(on_event));

  m_render_manager = RenderManager::Create();
  m_render_manager->init();
  m_scene_manager = SceneManager::Create();
  m_imgui_layer = std::make_unique<ImGuiLayer>();
  m_imgui_layer->on_attach();
}

Application::~Application() {
  ENGINE_WARN("Deleting application");
  m_render_manager->shutdown();
  m_imgui_layer->on_detach();
}

void Application::on_event(events::Event& e) {
  m_states_manager->get_current_state()->on_event(e);
}

void Application::pause(bool pause) {
  if (pause and not m_paused) {
    m_paused = true;
  } else if (not pause and m_paused) {
    m_paused = false;
  }
}

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
        const auto& current_state = m_states_manager->get_current_state();
        current_state->on_update(ts);
        m_scene_manager->on_update(ts);

        m_imgui_layer->on_imgui_update();
        current_state->on_imgui_update();
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