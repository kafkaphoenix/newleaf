#pragma once

#include <memory>
#include <string>

#include <imgui.h>

#include "../assets/assets_manager.h"
#include "../assets/model.h"
#include "../assets/prefab.h"
#include "../assets/texture.h"
#include "../settings/settings_manager.h"
#include "im_utils.h"

namespace nl {

std::string selected_asset_manager_tab_key;
std::string selected_asset_tab_type;
std::string selected_path;
char assets_text_filter[128]{}; // TODO: move to class

inline void
draw_assets_manager(const std::unique_ptr<AssetsManager>& assets_manager,
                    const std::unique_ptr<SettingsManager>& settings_manager) {
  const auto& assets = assets_manager->get_assets();

  if (assets.empty()) {
    ImGui::Text("no assets loaded");
    return;
  }

  int collapsed = collapser();

  ImGui::InputText("##filter", assets_text_filter,
                   IM_ARRAYSIZE(assets_text_filter));
  if (ImGui::IsItemHovered()) {
    ImGui::SetTooltip("Filter assets by name");
  }
  ImGui::SameLine();
  if (ImGui::Button("Clear Filter")) {
    assets_text_filter[0] = '\0';
  }

  ImGui::Separator();
  ImGui::Columns(2);

  for (const auto& [type, value] : assets) {
    if (collapsed not_eq -1) {
      ImGui::SetNextItemOpen(collapsed not_eq 0);
    }

    if (ImGui::CollapsingHeader(type.c_str())) {
      for (const auto& [name, _] : value) {
        if (assets_text_filter[0] not_eq '\0' and
            strstr(name.c_str(), assets_text_filter) == nullptr) {
          continue;
        }
        if (ImGui::Selectable(name.c_str())) {
          selected_asset_manager_tab_key = name;
          selected_asset_tab_type = type;
        }
      }
    }
  }

  if (collapsed == 0 or settings_manager->reload_scene) {
    selected_asset_manager_tab_key.clear();
    selected_asset_tab_type.clear();
    selected_path.clear();
  }

  ImGui::NextColumn();
  if (not selected_asset_manager_tab_key.empty()) {
    const auto& asset =
      assets.at(selected_asset_tab_type).at(selected_asset_manager_tab_key);
    const auto& assetInfo = asset->to_map();

    for (const auto& [key, value] : assetInfo) {
      if (key.starts_with("prototype_") and
          selected_asset_tab_type == "Prefab") {
        const auto& prefab =
          assets_manager->get<Prefab>(selected_asset_manager_tab_key);
        const auto& prototypeInfo = prefab->get_target_prototype_info(value);
        if (ImGui::TreeNode((selected_asset_tab_type +
                             selected_asset_manager_tab_key + key +
                             settings_manager->active_scene)
                              .c_str(),
                            key.c_str())) {
          for (const auto& [key, value] : prototypeInfo) {
            ImGui::BulletText("%s: %s", key.c_str(), value.c_str());
          }
          ImGui::TreePop();
        }
      } else if (key.starts_with("loaded_texture_") and
                 selected_asset_tab_type == "Model") {
        const auto& model =
          assets_manager->get<Model>(selected_asset_manager_tab_key);
        const auto& textureInfo = model->get_loaded_texture_info(value);
        if (ImGui::TreeNode((selected_asset_tab_type +
                             selected_asset_manager_tab_key + key +
                             settings_manager->active_scene)
                              .c_str(),
                            key.c_str())) {
          for (const auto& [key, value] : textureInfo) {
            ImGui::BulletText("%s: %s", key.c_str(), value.c_str());
          }
          ImGui::TreePop();
        }
      } else if (key.starts_with("path_") and
                 selected_asset_tab_type == "Texture") {
        const auto& texture =
          assets_manager->get<Texture>(selected_asset_manager_tab_key);
        if (texture->is_cubemap()) {
          ImGui::BulletText("%s: %s", key.c_str(), value.c_str());
        } else {
          ImGui::BulletText("%s: %s", key.c_str(), value.c_str());
          if (ImGui::IsItemHovered()) {
            selected_path = value;
          } else {
            selected_path.clear();
          }
        }
      } else {
        ImGui::BulletText("%s: %s", key.c_str(), value.c_str());
      }
    }

    if (not selected_path.empty()) {
      const auto& texture =
        assets_manager->get<Texture>(selected_asset_manager_tab_key);
      if (not texture->is_cubemap()) {
        uint32_t maxWidth = texture->get_width();
        uint32_t maxHeight = texture->get_height();
        if (maxWidth <= 64 and maxHeight <= 64) {
          maxWidth *= 2;
          maxHeight *= 2;
        } else if (maxWidth >= 1024 or maxHeight >= 1024) {
          maxHeight /= 4;
          maxWidth /= 4;
        } else if (maxWidth >= 512 or maxHeight >= 512) {
          maxHeight /= 3;
          maxWidth /= 3;
        } else if (maxWidth >= 256 or maxHeight >= 256) {
          maxHeight /= 2;
          maxWidth /= 2;
        }
        ImGui::Image((void*)texture->get_id(), ImVec2(maxWidth, maxHeight),
                     ImVec2(0, 1), ImVec2(1, 0));
      }
    }
  }

  ImGui::Columns(1);
}
}
