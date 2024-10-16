#pragma once

#include "core/layersManager.h"
#include "core/time.h"
#include "events/event.h"
#include "pch.h"

namespace potatoengine {

class State {
  public:
    State(std::string&& name = "TemplateState") : m_name(std::move(name)) {
      m_layersManager = LayersManager::Create();
    }
    virtual ~State() = default;

    virtual void onAttach() {}
    virtual void onDetach() {}
    virtual void onUpdate(const Time&) {}
    virtual void onImguiUpdate() {}
    virtual void onEvent(events::Event&) {}

    std::string_view getName() const { return m_name; }
    const std::unique_ptr<LayersManager>& getLayersManager() const {
      return m_layersManager;
    }
    void clearLayers() { m_layersManager->clear(); }

  protected:
    std::string m_name;
    std::unique_ptr<LayersManager> m_layersManager;
};

}