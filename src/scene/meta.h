#pragma once

#include <entt/entt.hpp>

namespace potatoengine {

template <typename Component, typename... Args>
inline Component& assign(entt::entity e, Args... args) {
  auto& registry = Application::Get().get_scene_manager()->get_registry();
  ENGINE_ASSERT(not registry.all_of<Component>(e), "Entity already has component {}",
                typeid(Component).name());
  return registry.emplace<Component>(e, std::forward<Args>(args)...);
}

template <typename Component>
inline Component& on_component_added(entt::entity e, Component& c) {
  Application::Get().get_scene_manager()->on_component_added<Component>(e, c);
  return c;
}

template <typename Component>
inline Component& on_component_cloned(entt::entity e, Component& c) {
  Application::Get().get_scene_manager()->on_component_cloned<Component>(e, c);
  return c;
}
}