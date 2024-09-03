#pragma once

#include "../core/layer.h"

namespace nl {

class ImGuiLayer : public Layer {
  public:
    ImGuiLayer();

    virtual void on_attach() override;
    virtual void on_detach() override;
    virtual void on_imgui_update() override;
    static void begin();
    static void end();
};
}