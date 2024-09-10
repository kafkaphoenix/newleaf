#pragma once

#include "../events/event.h"
#include "../utils/time.h"
#include <string>
#include <string_view>

namespace nl {

class Layer {
  public:
    Layer(std::string&& name = "template_layer") : m_name(std::move(name)) {}
    virtual ~Layer() = default;

    virtual void on_attach() {}
    virtual void on_detach() {}
    virtual void on_update(const Time&) {}
    virtual void on_imgui_update() {}
    virtual void on_event(events::Event&) {}

    std::string_view get_name() const { return m_name; }
    bool is_enabled() const { return m_enabled; }
    void enable(bool enabled) { m_enabled = enabled; }

  protected:
    std::string m_name;
    bool m_enabled{};
};

}