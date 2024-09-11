#pragma once

#include "../utils/numeric_comparator.h"
#include "state.h"

#include <map>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

namespace nl {

class StatesManager {
  public:
    ~StatesManager();

    void push_state(std::unique_ptr<State>&& s);
    void pop_state(std::string_view name);
    void push_layer(std::unique_ptr<Layer>&& l);
    void push_overlay(std::unique_ptr<Layer>&& o, bool enabled);
    void enable_overlay(std::string_view name);
    void disable_overlay(std::string_view name);
    const std::unique_ptr<State>& get_current_state();
    uint32_t get_state_index();

    const std::map<std::string, std::string, NumericComparator>& get_metrics();

    static std::unique_ptr<StatesManager> create();

  private:
    std::vector<std::unique_ptr<State>> m_states;
    uint32_t m_index{};
    std::map<std::string, std::string, NumericComparator> m_metrics;
    bool m_dirty{};
};

}