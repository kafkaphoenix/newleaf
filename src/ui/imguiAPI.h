#pragma once

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <imgui_stdlib.h>

#include "pch.h"
#include "ui/imutils.h"

namespace potatoengine::ui {
class ImGuiAPI {
  public:
    static void Init(GLFWwindow* window, uint32_t openglMajorVersion,
                     uint32_t openglMinorVersion);
    static void NewFrame();
    static void Render();
    static void Shutdown();
};
}