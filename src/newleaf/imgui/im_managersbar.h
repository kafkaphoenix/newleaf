#pragma once

#include <imgui.h>

#include "../assets/assets_manager.h"
#include "../graphics/render_manager.h"
#include "../scene/scene_manager.h"
#include "../settings/settings_manager.h"
#include "../state/states_manager.h"
#include "im_assets_manager.h"
#include "im_render_manager.h"
#include "im_scene_manager.h"
#include "im_settings_manager.h"
#include "im_states_manager.h"

#include <memory>

namespace nl {

inline void
draw_managersbar(const std::unique_ptr<AssetsManager>& assets_manager,
                 const std::unique_ptr<RenderManager>& render_manager,
                 const std::unique_ptr<SceneManager>& scene_manager,
                 const std::unique_ptr<SettingsManager>& settings_manager,
                 const std::unique_ptr<StatesManager>& states_manager) {
  if (ImGui::BeginTabBar("RootTabBar", ImGuiTabBarFlags_Reorderable)) {
    if (ImGui::BeginTabItem("Assets Manager")) {
      draw_assets_manager(assets_manager, settings_manager);
      ImGui::EndTabItem();
    } else {
      selected_asset_manager_tab_key.clear();
      selected_asset_tab_type.clear();
      selected_path.clear();
      assets_text_filter[0] = '\0';
    }
    if (ImGui::BeginTabItem("Scene Manager")) {
      draw_scene_manager(scene_manager, settings_manager);
      ImGui::EndTabItem();
    } else {
      selected_scene_manager_tabkey.clear();
      scene_objects_text_filter[0] = '\0';
      filter_prefabs = false;
      filter_prototypes = false;
      filter_instances = false;
      filter_components = false;
      filter_systems = false;
    }
    if (ImGui::BeginTabItem("Render Manager")) {
      draw_render_manager(render_manager, settings_manager);
      ImGui::EndTabItem();
    } else {
      selected_render_manager_tabkey.clear();
      selected_render_manager_tabtype.clear();
      render_objects_text_filter[0] = '\0';
      filter_fbos = false;
      filter_shader_programs = false;
      filter_shader_info = false;
    }
    if (ImGui::BeginTabItem("States Manager")) {
      draw_states_manager(settings_manager, states_manager);
      ImGui::EndTabItem();
    } else {
      states_text_filter[0] = '\0';
      filter_states = false;
      filter_layers = false;
    }
    if (ImGui::BeginTabItem("Settings Manager")) {
      draw_settings_manager(settings_manager, render_manager);
      ImGui::EndTabItem();
    } else {
      selected_settings_manager_tabkey.clear();
    }
    ImGui::EndTabBar();
  }
}
}