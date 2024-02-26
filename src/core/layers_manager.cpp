#include "core/layers_manager.h"

namespace nl {

LayersManager::~LayersManager() {
  ENGINE_WARN("Deleting layers manager");
  for (auto& l : m_layers) {
    if (l->is_enabled()) {
      l->on_detach();
    }
  }
  m_layers.clear();
}

void LayersManager::push_layer(std::unique_ptr<Layer>&& l) {
  l->on_attach();
  l->enable(true);
  m_layers.emplace(m_layers.begin() + m_index++, std::move(l));
}

void LayersManager::push_overlay(std::unique_ptr<Layer>&& o, bool enabled) {
  if (enabled) {
    o->on_attach();
    o->enable(true);
  }
  m_layers.emplace_back(std::move(o));
}

void LayersManager::enable_overlay(std::string_view name) {
  auto it = std::ranges::find_if(
    m_layers | std::views::drop(m_index),
    [&](const auto& layer) { return layer->get_name() == name; });
  if (it not_eq m_layers.end()) {
    if (not(*it)->is_enabled()) {
      (*it)->on_attach();
      (*it)->enable(true);
    } else {
      ENGINE_WARN("Overlay {} is already enabled", name);
    }
  }
}

void LayersManager::disable_overlay(std::string_view name) {
  auto it = std::ranges::find_if(
    m_layers | std::views::drop(m_index),
    [&](const auto& layer) { return layer->get_name() == name; });
  if (it not_eq m_layers.end()) {
    if ((*it)->is_enabled()) {
      (*it)->on_detach();
      (*it)->enable(false);
    } else {
      ENGINE_WARN("Overlay {} is already disabled", name);
    }
  }
}

void LayersManager::clear() {
  for (auto& l : m_layers) {
    if (l->is_enabled()) {
      l->on_detach();
    }
  }
  m_layers.clear();
  m_index = 0;
}

std::unique_ptr<LayersManager> LayersManager::Create() {
  return std::make_unique<LayersManager>();
}
}
