#include "layers_manager.h"

#include <ranges>

#include "../logging/log_manager.h"

namespace nl {

LayersManager::LayersManager() { ENGINE_TRACE("initializing layers manager"); }

LayersManager::~LayersManager() {
  ENGINE_WARN("deleting layers manager");
  for (auto& l : m_layers) {
    if (l->is_enabled()) {
      ENGINE_WARN("detaching layer {}", l->get_name());
      l->on_detach();
    }
  }
  m_layers.clear();
}

void LayersManager::push_layer(std::unique_ptr<Layer>&& l) {
  if (contains_layer(l->get_name())) {
    ENGINE_ERROR("layer {} already exists in layers manager, not pushing", l->get_name());
    return;
  }
  l->on_attach();
  l->enable(true);
  m_layers.emplace(m_layers.begin() + m_index++, std::move(l));
}

void LayersManager::push_overlay(std::unique_ptr<Layer>&& o, bool enabled) {
  if (contains_overlay(o->get_name())) {
    ENGINE_ERROR("overlay {} already exists in layers manager, not pushing", o->get_name());
    return;
  }
  if (enabled) {
    o->on_attach();
    o->enable(true);
  }
  m_layers.emplace_back(std::move(o));
}

bool LayersManager::contains_layer(std::string_view name) const {
  return std::ranges::find_if(m_layers, [&](const auto& layer) { return layer->get_name() == name; }) not_eq
         m_layers.end();
}

bool LayersManager::contains_overlay(std::string_view name) const {
  return std::ranges::find_if(m_layers | std::views::drop(m_index),
                              [&](const auto& layer) { return layer->get_name() == name; }) not_eq m_layers.end();
}

bool LayersManager::is_overlay_enabled(std::string_view name) const {
  auto it = std::ranges::find_if(m_layers | std::views::drop(m_index),
                                 [&](const auto& layer) { return layer->get_name() == name; });
  if (it not_eq m_layers.end()) {
    return (*it)->is_enabled();
  }
  return false;
}

void LayersManager::enable_overlay(std::string_view name) {
  auto it = std::ranges::find_if(m_layers | std::views::drop(m_index),
                                 [&](const auto& layer) { return layer->get_name() == name; });
  if (it == m_layers.end()) {
    ENGINE_ERROR("overlay {} not found in layers manager, cannot enable", name);
    return;
  } else {
    if (not(*it)->is_enabled()) {
      (*it)->on_attach();
      (*it)->enable(true);
    } else {
      ENGINE_WARN("overlay {} is already enabled", name);
    }
  }
}

void LayersManager::disable_overlay(std::string_view name) {
  auto it = std::ranges::find_if(m_layers | std::views::drop(m_index),
                                 [&](const auto& layer) { return layer->get_name() == name; });
  if (it == m_layers.end()) {
    ENGINE_ERROR("overlay {} not found in layers manager, cannot disable", name);
    return;
  } else {
    if ((*it)->is_enabled()) {
      (*it)->on_detach();
      (*it)->enable(false);
    } else {
      ENGINE_WARN("overlay {} is already disabled", name);
    }
  }
}

void LayersManager::clear() {
  ENGINE_DEBUG("clearing layers manager");
  for (auto& l : m_layers) {
    if (l->is_enabled()) {
      ENGINE_DEBUG("detaching layer {}", l->get_name());
      l->on_detach();
    }
  }
  m_layers.clear();
  m_index = 0;
}

std::unique_ptr<LayersManager> LayersManager::create() { return std::make_unique<LayersManager>(); }
}
