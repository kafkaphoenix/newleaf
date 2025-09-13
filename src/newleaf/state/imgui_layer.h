#pragma once

#include <memory>

#include "layer.h"

namespace nl {

class ImGuiLayer : public Layer {
  public:
    ImGuiLayer();

    virtual void on_attach() override;
    virtual void on_detach() override;
    virtual void on_imgui_update() override;

    static void begin();
    static void end();
    static std::unique_ptr<Layer> create();
};
}