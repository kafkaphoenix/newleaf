#pragma once

#include <memory>
#include <string>
#include <string_view>

#include "../events/event.h"
#include "../utils/time.h"
#include "layers_manager.h"

namespace nl {

class State {
  public:
    State(std::string&& name = "template_state") : m_name(std::move(name)) {
      m_layers_manager = LayersManager::create();
    }
    virtual ~State() = default;

    virtual void on_attach() {}
    virtual void on_detach() {}
    virtual void on_update(const Time&) {}
    virtual void on_imgui_update() {}
    virtual void on_event(Event&) {}

    std::string_view get_name() const { return m_name; }
    LayersManager& get_layers_manager() {
      return *m_layers_manager;
    }
    const LayersManager& get_layers_manager() const {
      return *m_layers_manager;
    }
    void clear_layers() { m_layers_manager->clear(); }

  protected:
    std::string m_name;
    std::unique_ptr<LayersManager> m_layers_manager;
};

}