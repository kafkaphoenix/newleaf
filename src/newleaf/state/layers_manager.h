#pragma once

#include "layer.h"

#include <memory>
#include <string>
#include <string_view>
#include <vector>

namespace nl {

class LayersManager {
  public:
    ~LayersManager();

    void push_layer(std::unique_ptr<Layer>&& l);
    void push_overlay(std::unique_ptr<Layer>&& o, bool enabled);
    void enable_overlay(std::string_view name);
    void disable_overlay(std::string_view name);
    std::vector<std::unique_ptr<Layer>>& get_layers() {
      return m_layers;
    }
    void clear();

    auto begin() { return m_layers.begin(); }
    auto end() { return m_layers.end(); }
    auto rbegin() { return m_layers.rbegin(); }
    auto rend() { return m_layers.rend(); }

    auto begin() const { return m_layers.begin(); }
    auto end() const { return m_layers.end(); }
    auto rbegin() const { return m_layers.rbegin(); }
    auto rend() const { return m_layers.rend(); }

    static std::unique_ptr<LayersManager> create();

  private:
    std::vector<std::unique_ptr<Layer>> m_layers;
    uint32_t m_index{};
};

}