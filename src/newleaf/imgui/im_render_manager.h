#pragma once

#include <memory>
#include <string>

#include <imgui.h>

#include "../assets/assets_manager.h"
#include "../graphics/render_manager.h"
#include "../settings/settings_manager.h"
#include "../utils/map_json_serializer.h"
#include "im_utils.h"

namespace nl {

std::string selected_render_manager_tabkey;
std::string selected_render_manager_tabtype;
char render_objects_text_filter[128]{}; // TODO: move to class
bool filter_fbos{};
bool filter_shaders{};
bool filter_shader_info{};

inline void draw_render_manager(const RenderManager& render_manager, const AssetsManager& assets_manager,
                                const SettingsManager& settings_manager) {
  const auto& fbos = render_manager.get_framebuffers();
  const auto& assets = assets_manager.get_assets();
  const auto shaders_it = assets.find("Shader");
  const bool has_shaders = shaders_it != assets.end();

  int collapsed = collapser();

  ImGui::InputText("##filter", render_objects_text_filter, IM_ARRAYSIZE(render_objects_text_filter));
  if (ImGui::IsItemHovered()) {
    ImGui::SetTooltip("Filter render objects by name");
  }
  ImGui::SameLine();
  if (ImGui::Button("Clear Filter")) {
    render_objects_text_filter[0] = '\0';
  }
  ImGui::Checkbox("FBOs##1", &filter_fbos);
  ImGui::SameLine();
  ImGui::Checkbox("Shaders##1", &filter_shaders);
  ImGui::SameLine();
  ImGui::Checkbox("Shader info##1", &filter_shader_info);

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

  if (ImGui::CollapsingHeader("Shaders")) {
    if (not has_shaders or shaders_it->second.empty()) {
      ImGui::Text("No shaders");
    }
    if (has_shaders) {
      for (const auto& [key, value] : shaders_it->second) {
        if (filter_shaders and render_objects_text_filter[0] not_eq '\0' and
            strstr(key.c_str(), render_objects_text_filter) == nullptr) {
          continue;
        }
        if (ImGui::Selectable(key.c_str())) {
          selected_render_manager_tabkey = key;
          selected_render_manager_tabtype = "Shader";
        }
      }
    }
  }

  if (collapsed == 0 or settings_manager.reload_scene) {
    selected_render_manager_tabkey.clear();
    selected_render_manager_tabtype.clear();
  }

  ImGui::NextColumn();
  if (not selected_render_manager_tabkey.empty()) {
    if (selected_render_manager_tabtype == "Shader") {
      if (has_shaders) {
        const auto& shader = shaders_it->second.at(selected_render_manager_tabkey);
        const auto& shader_info = shader->to_map();
        for (const auto& [key, value] : shader_info) {
          if (filter_shader_info and render_objects_text_filter[0] not_eq '\0' and
              strstr(key.c_str(), render_objects_text_filter) == nullptr) {
            continue;
          }
          ImGui::BulletText("%s: %s", key.c_str(), value.c_str());
        }
      }
    } else if (selected_render_manager_tabtype == "Framebuffers") {
      const auto& fbo_value = fbos.at(selected_render_manager_tabkey);
      const auto& fbo_info = fbo_value->to_map();
      for (const auto& [key, value] : fbo_info) {
        if (key == "color_texture" or key == "depth_texture") {
          const auto& texture_info = json_to_map(value);
          if (ImGui::TreeNode(
                (selected_render_manager_tabtype + selected_render_manager_tabkey + key + settings_manager.active_scene)
                  .c_str(),
                key.c_str())) {
            for (const auto& [k, v] : texture_info) {
              ImGui::BulletText("%s: %s", k.c_str(), v.c_str());
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
