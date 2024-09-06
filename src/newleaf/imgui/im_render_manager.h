#pragma once

#include <imgui.h>

#include "../core/settings_manager.h"
#include "../render/render_manager.h"
#include "../utils/map_json_serializer.h"
#include "im_utils.h"

#include <memory>
#include <string>

namespace nl {

std::string selected_render_manager_tabkey;
std::string selected_render_manager_tabtype;
char render_objects_text_filter[128]{}; // TODO: move to class
bool filter_fbos{};
bool filter_shader_programs{};
bool filter_shader_info{};

inline void
draw_render_manager(const std::unique_ptr<RenderManager>& render_manager,
                    const std::unique_ptr<SettingsManager>& settings_manager) {
  const auto& fbos = render_manager->get_framebuffers();
  const auto& sp = render_manager->get_shader_programs();

  int collapsed = collapser();

  ImGui::InputText("##filter", render_objects_text_filter,
                   IM_ARRAYSIZE(render_objects_text_filter));
  if (ImGui::IsItemHovered()) {
    ImGui::SetTooltip("Filter render objects by name");
  }
  ImGui::SameLine();
  if (ImGui::Button("Clear Filter")) {
    render_objects_text_filter[0] = '\0';
  }
  ImGui::Checkbox("FBOS", &filter_fbos);
  ImGui::SameLine();
  ImGui::Checkbox("Shader programs", &filter_shader_programs);
  ImGui::SameLine();
  ImGui::Checkbox("Shader info", &filter_shader_info);

  ImGui::Separator();
  ImGui::Columns(2);

  if (collapsed not_eq -1) {
    ImGui::SetNextItemOpen(collapsed not_eq 0);
  }

  if (ImGui::CollapsingHeader("Framebuffers")) {
    if (fbos.empty()) {
      ImGui::Text("No framebuffers");
    }
    for (const auto& [key, value] : fbos) {
      if (filter_fbos and render_objects_text_filter[0] not_eq '\0' and
          strstr(key.c_str(), render_objects_text_filter) == nullptr) {
        continue;
      }
      if (ImGui::Selectable(key.c_str())) {
        selected_render_manager_tabkey = key;
        selected_render_manager_tabtype = "Framebuffers";
      }
    }
  }

  if (collapsed not_eq -1) {
    ImGui::SetNextItemOpen(collapsed not_eq 0);
  }

  if (ImGui::CollapsingHeader("Shader programs")) {
    if (sp.empty()) {
      ImGui::Text("No shader programs");
    }
    for (const auto& [key, value] : sp) {
      if (filter_shader_programs and
          render_objects_text_filter[0] not_eq '\0' and
          strstr(key.c_str(), render_objects_text_filter) == nullptr) {
        continue;
      }
      if (ImGui::Selectable(key.c_str())) {
        selected_render_manager_tabkey = key;
        selected_render_manager_tabtype = "Shader Program";
      }
    }
  }

  if (collapsed == 0 or settings_manager->reload_scene) {
    selected_render_manager_tabkey.clear();
    selected_render_manager_tabtype.clear();
  }

  ImGui::NextColumn();
  if (not selected_render_manager_tabkey.empty()) {
    if (selected_render_manager_tabtype == "Shader Program") {
      const auto& shaderProgram = sp.at(selected_render_manager_tabkey);
      const auto& shaderProgramInfo = shaderProgram->to_map();
      for (const auto& [key, value] : shaderProgramInfo) {
        if (filter_shader_info and render_objects_text_filter[0] not_eq '\0' and
            strstr(key.c_str(), render_objects_text_filter) == nullptr) {
          continue;
        }
        ImGui::BulletText("%s: %s", key.c_str(), value.c_str());
      }
    } else if (selected_render_manager_tabtype == "Framebuffers") {
      const auto& value = fbos.at(selected_render_manager_tabkey);
      const auto& fboInfo = value->to_map();
      for (const auto& [key, value] : fboInfo) {
        if (key == "Color texture" or key == "Depth texture") {
          const auto& textureInfo = json_to_map(value);
          if (ImGui::TreeNode((selected_render_manager_tabtype +
                               selected_render_manager_tabkey + key +
                               settings_manager->active_scene)
                                .c_str(),
                              key.c_str())) {
            for (const auto& [key, value] : textureInfo) {
              ImGui::BulletText("%s: %s", key.c_str(), value.c_str());
            }
            ImGui::TreePop();
          }
        } else {
          ImGui::BulletText("%s: %s", key.c_str(), value.c_str());
        }
      }
    }
  }

  ImGui::Columns(1);
}
}
