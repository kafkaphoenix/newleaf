#pragma once

#include <memory>

#include "layer.h"

namespace nl {

class ImGuiLayer : public Layer {
  public:
    ImGuiLayer();

    void on_attach() override;
    void on_detach() override;
    void on_imgui_update() override;

    static void begin();
    static void end();
    static std::unique_ptr<Layer> create();
};
}