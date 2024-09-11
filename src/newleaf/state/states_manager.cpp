#include "states_manager.h"

#include <ranges>

#include "../logging/log_manager.h"
#include "../utils/assert.h"
#include "../utils/map_json_serializer.h"

namespace nl {

StatesManager::~StatesManager() {
  ENGINE_WARN("deleting states manager");
  for (auto& s : m_states) {
    s->on_detach();
  }
  m_states.clear();
  m_metrics.clear();
  m_dirty = false;
}

void StatesManager::push_state(std::unique_ptr<State>&& s) {
  if (m_index > 0) {
    // TODO rethink this
    m_states[m_index - 1]->on_detach();
  }
  m_states.emplace(m_states.begin() + m_index++, std::move(s));
  m_states[m_index - 1]->on_attach();
  m_dirty = true;
}

void StatesManager::pop_state(std::string_view name) {
  auto it = std::ranges::find_if(
    m_states | std::views::take(m_index),
    [&](const auto& state) { return state->get_name() == name; });
  if (it not_eq m_states.begin() + m_index) {
    (*it)->on_detach();
    m_states.erase(it);
    --m_index;
    if (m_index > 0) {
      // TODO rethink this
      m_states[m_index - 1]->on_attach();
    }
  }
  m_dirty = true;
}

void StatesManager::push_layer(std::unique_ptr<Layer>&& l) {
  ENGINE_ASSERT(m_index > 0, "no states to push layer to");
  m_states[m_index - 1]->get_layers_manager()->push_layer(std::move(l));
  m_dirty = true;
}

void StatesManager::push_overlay(std::unique_ptr<Layer>&& o, bool enabled) {
  ENGINE_ASSERT(m_index > 0, "no states to push overlay to");
  m_states[m_index - 1]->get_layers_manager()->push_overlay(std::move(o),
                                                            enabled);
  m_dirty = true;
}

void StatesManager::enable_overlay(std::string_view name) {
  ENGINE_ASSERT(m_index > 0, "no states to enable overlay in");
  m_states[m_index - 1]->get_layers_manager()->enable_overlay(name);
  m_dirty = true;
}

void StatesManager::disable_overlay(std::string_view name) {
  ENGINE_ASSERT(m_index > 0, "no states to disable overlay in");
  m_states[m_index - 1]->get_layers_manager()->disable_overlay(name);
  m_dirty = true;
}

const std::unique_ptr<State>& StatesManager::get_current_state() {
  ENGINE_ASSERT(m_index > 0, "no states to get current state from");
  return m_states[m_index - 1];
}

uint32_t StatesManager::get_state_index() { return m_index; }

const std::map<std::string, std::string, NumericComparator>&
StatesManager::get_metrics() {
  if (not m_dirty) {
    return m_metrics;
  }

  m_metrics.clear();
  for (const auto& state : m_states | std::views::take(m_index)) {
    std::map<std::string, std::string, NumericComparator> layers;
    for (const auto& layer : state->get_layers_manager()->get_layers()) {
      layers[layer->get_name().data()] =
        layer->is_enabled() ? "enabled" : "disabled";
    }
    m_metrics[state->get_name().data()] = map_to_json(layers);
  }
  m_dirty = false;

  return m_metrics;
}

std::unique_ptr<StatesManager> StatesManager::create() {
  return std::make_unique<StatesManager>();
}
}
