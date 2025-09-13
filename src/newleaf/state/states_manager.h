#pragma once

#include <map>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include "../utils/numeric_comparator.h"
#include "state.h"

namespace nl {

class StatesManager {
  public:
    StatesManager();
    ~StatesManager();

    void push_state(std::unique_ptr<State>&& s);
    void pop_state(std::string_view name);
    bool contains_state(std::string_view name) const;
    void push_layer(std::unique_ptr<Layer>&& l);
    bool contains_layer(std::string_view name) const;
    void push_overlay(std::unique_ptr<Layer>&& o, bool enabled);
    void enable_overlay(std::string_view name);
    void disable_overlay(std::string_view name);
    bool is_overlay_enabled(std::string_view name) const;
    bool contains_overlay(std::string_view name) const;
    void clear_layers();
    State& get_current_state();
    const State& get_current_state() const;

    std::map<std::string, std::string, NumericComparator>& compute_metrics();

    static std::unique_ptr<StatesManager> create();

  private:
    std::vector<std::unique_ptr<State>> m_states;
    uint32_t m_index{};
    std::map<std::string, std::string, NumericComparator> m_metrics;
    bool m_dirty{};
};

}