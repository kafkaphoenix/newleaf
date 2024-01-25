#pragma once

#include <entt/entt.hpp>
#include <imgui.h>

#include "engineAPI.h"

namespace demos::dispatchers {

bool onWindowClosed(engine::events::WindowCloseEvent& e) {
  engine::Application::Get().close();
  return true;
}

bool onWindowRestored(engine::events::WindowRestoredEvent& e) {
  return true;
}

bool onWindowMinimized(engine::events::WindowMinimizedEvent& e) {
  return true;
}

bool onWindowMaximized(engine::events::WindowMaximizedEvent& e) { 
  return true;
}

bool onWindowFocus(engine::events::WindowFocusEvent& e) {
  engine::Application::Get().pause(false);
  if (engine::Application::Get().isRestoreGamePaused()) {
    engine::Application::Get().togglePauseGame(true);
    engine::Application::Get().setRestoreGamePaused(false);
  } else {
    engine::Application::Get().togglePauseGame(false);
  }
  return true;
}

bool onWindowLostFocus(engine::events::WindowLostFocusEvent& e) {
  engine::Application::Get().pause(true);
  if (engine::Application::Get().isGamePaused()) {
    engine::Application::Get().setRestoreGamePaused(true);
  } else {
    engine::Application::Get().togglePauseGame(true);
  }
  return true;
}

bool onWindowMoved(engine::events::WindowMovedEvent& e) { return true; }

bool onWindowResized(engine::events::WindowResizeEvent& e,
                     entt::registry& registry) {
  engine::RendererAPI::SetViewport(0, 0, e.getWidth(), e.getHeight());
  auto& app = engine::Application::Get();
  const auto& settings = app.getSettings();
  if (not settings->fullscreen) { // when resizing with the mouse
    settings->windowWidth = e.getWidth();
    settings->windowHeight = e.getHeight();
  }

  const auto& renderer = app.getRenderer();
  if (not renderer->getFramebuffers().empty()) {
    entt::entity fbo = registry.view<engine::CFBO, engine::CUUID>()
                         .front(); // TODO: support more than one?
    engine::CFBO& cfbo = registry.get<engine::CFBO>(fbo);
    renderer->deleteFramebuffer(cfbo.fbo);
    renderer->addFramebuffer(std::string(cfbo.fbo), e.getWidth(), e.getHeight(),
                             cfbo.attachment);
  }

  return true;
}

bool onAppTick(engine::events::AppTickEvent& e) { return true; }

bool onAppUpdate(engine::events::AppUpdateEvent& e) { return true; }

bool onAppRender(engine::events::AppRenderEvent& e) { return true; }

void appDispatcher(entt::registry& registry, engine::events::Event& e) {
  engine::events::EventDispatcher dispatcher(e);

  dispatcher.dispatch<engine::events::WindowCloseEvent>(
    BIND_STATIC_EVENT(onWindowClosed));
  dispatcher.dispatch<engine::events::WindowRestoredEvent>(
    BIND_STATIC_EVENT(onWindowRestored));
  dispatcher.dispatch<engine::events::WindowMinimizedEvent>(
    BIND_STATIC_EVENT(onWindowMinimized));
  dispatcher.dispatch<engine::events::WindowMaximizedEvent>(
    BIND_STATIC_EVENT(onWindowMaximized));
  dispatcher.dispatch<engine::events::WindowResizeEvent>(
    BIND_STATIC_EVENT(onWindowResized, registry));
  dispatcher.dispatch<engine::events::WindowFocusEvent>(
    BIND_STATIC_EVENT(onWindowFocus));
  dispatcher.dispatch<engine::events::WindowLostFocusEvent>(
    BIND_STATIC_EVENT(onWindowLostFocus));
  dispatcher.dispatch<engine::events::WindowMovedEvent>(
    BIND_STATIC_EVENT(onWindowMoved));

  dispatcher.dispatch<engine::events::AppTickEvent>(
    BIND_STATIC_EVENT(onAppTick));
  dispatcher.dispatch<engine::events::AppUpdateEvent>(
    BIND_STATIC_EVENT(onAppUpdate));
  dispatcher.dispatch<engine::events::AppRenderEvent>(
    BIND_STATIC_EVENT(onAppRender));
}

}