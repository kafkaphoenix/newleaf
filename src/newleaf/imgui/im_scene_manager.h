#pragma once

#include <entt/entt.hpp>
#include <imgui.h>

#include "../core/settings_manager.h"
#include "../imgui/im_utils.h"
#include "../pch.h"
#include "../scene/scene_manager.h"
#include "../utils/map_json_serializer.h"
#include "../utils/numeric_comparator.h"

using namespace entt::literals;

namespace nl {

std::string selected_scene_manager_tabkey;
char scene_objects_text_filter[128]{}; // TODO: move to class
bool filter_prefabs{};
bool filter_prototypes{};
bool filter_instances{};
bool filter_components{};
bool filter_systems{};

inline void draw_leaf_info(
  const std::map<std::string, std::string, NumericComparator>& info,
  const std::string& sceneName) {
  for (const auto& [k, v] : info) {
    if (k.starts_with("mesh ") or k.starts_with("texture ") or
        k.starts_with("material ") or k.starts_with("transform ") or
        (k.starts_with("vao ") and v != "undefined")) {
      if (ImGui::TreeNode((k + sceneName).c_str(), k.c_str())) {
        auto childInfoData = json_to_map(v);
        for (const auto& [key, value] : childInfoData) {
          if (key.starts_with("texture ") or
              (key.starts_with("vao ") and value != "undefined")) {
            if (ImGui::TreeNode((key + sceneName).c_str(), key.c_str())) {
              // CBody, CShape, CChunk have a CMesh that has a VAO and CTexture
              auto rechildInfoData = json_to_map(value);
              for (const auto& [key, value] : rechildInfoData) {
                ImGui::BulletText("%s: %s", key.c_str(), value.c_str());
              }
              ImGui::TreePop();
            }
          } else {
            ImGui::BulletText("%s: %s", key.c_str(), value.c_str());
          }
        }
        ImGui::TreePop();
      }
    } else {
      ImGui::BulletText("%s: %s", k.c_str(), v.c_str());
    }
  }
}

inline void
draw_scene_manager(const std::unique_ptr<SceneManager>& scene_manager,
                   const std::unique_ptr<SettingsManager>& settings_manager) {
  entt::registry& registry = scene_manager->get_registry();

  if (registry.storage<entt::entity>().in_use() == 0) {
    ImGui::Text("No entities loaded");
    return;
  }

  int collapsed = collapser();

  ImGui::InputText("##filter", scene_objects_text_filter,
                   IM_ARRAYSIZE(scene_objects_text_filter));
  if (ImGui::IsItemHovered()) {
    ImGui::SetTooltip("Filter entities or components by name");
  }
  ImGui::SameLine();
  if (ImGui::Button("Clear Filter")) {
    scene_objects_text_filter[0] = '\0';
  }
  ImGui::Checkbox("Prefabs", &filter_prefabs);
  ImGui::SameLine();
  ImGui::Checkbox("Prototypes", &filter_prototypes);
  ImGui::SameLine();
  ImGui::Checkbox("Instances", &filter_instances);
  ImGui::SameLine();
  ImGui::Checkbox("Components", &filter_components);
  ImGui::SameLine();
  ImGui::Checkbox("Systems", &filter_systems);

  ImGui::Separator();
  ImGui::Columns(2);

  if (collapsed not_eq -1) {
    ImGui::SetNextItemOpen(collapsed not_eq 0);
  }

  if (ImGui::CollapsingHeader("Prefabs")) {
    for (const auto& [prefab_id, prototypes] :
         scene_manager->get_all_prototypes()) {
      std::string prefabName = "Prefab " + prefab_id;
      if (filter_prefabs and scene_objects_text_filter[0] not_eq '\0' and
          strstr(prefabName.c_str(), scene_objects_text_filter) == nullptr) {
        continue;
      }
      if (ImGui::TreeNode((prefabName + settings_manager->active_scene).c_str(),
                          prefabName.c_str())) {
        for (auto& [prototype_id, entity] : prototypes) {
          if (filter_prototypes and scene_objects_text_filter[0] not_eq '\0' and
              strstr(prototype_id.c_str(), scene_objects_text_filter) ==
                nullptr) {
            continue;
          }
          if (ImGui::Selectable(prototype_id.c_str())) {
            selected_scene_manager_tabkey =
              std::to_string(entt::to_integral(entity));
          }
        }
        ImGui::TreePop();
      }
    }
  }

  if (collapsed not_eq -1) {
    ImGui::SetNextItemOpen(collapsed not_eq 0);
  }

  if (ImGui::CollapsingHeader("Instances")) {
    for (const auto& [name, entity] : scene_manager->get_named_entities()) {
      if (filter_instances and scene_objects_text_filter[0] not_eq '\0' and
          strstr(name.c_str(), scene_objects_text_filter) == nullptr) {
        continue;
      }
      if (ImGui::Selectable(name.c_str())) {
        selected_scene_manager_tabkey =
          std::to_string(entt::to_integral(entity));
      }
    }
  }

  if (collapsed not_eq -1) {
    ImGui::SetNextItemOpen(collapsed not_eq 0);
  }

  if (ImGui::CollapsingHeader("Systems")) {
    for (const auto& name : scene_manager->get_named_systems()) {
      if (filter_systems and scene_objects_text_filter[0] not_eq '\0' and
          strstr(name.c_str(), scene_objects_text_filter) == nullptr) {
        continue;
      }
      ImGui::Text("%s", name.c_str());
    }
  }

  if (collapsed == 0 or settings_manager->reload_scene) {
    selected_scene_manager_tabkey.clear();
  }

  ImGui::NextColumn();
  if (not selected_scene_manager_tabkey.empty()) {
    entt::entity entity =
      entt::entity(std::stoul(selected_scene_manager_tabkey));
    if (registry.valid(entity)) {
      ImGui::SeparatorText("Components");
      std::string cName;
      entt::meta_type cType;
      entt::meta_any cData;
      entt::meta_func to_map_func;
      entt::meta_any data_;
      std::map<std::string, std::string, NumericComparator> info;
      for (auto&& [id, storage] : registry.storage()) {
        if (storage.contains(entity)) {
          cName = storage.type().name();
          cName = cName.substr(cName.find_last_of(':') + 1);
          if (filter_components and scene_objects_text_filter[0] not_eq '\0' and
              strstr(cName.c_str(), scene_objects_text_filter) == nullptr) {
            continue;
          }
          cType = entt::resolve(storage.type());
          cData = cType.construct(storage.value(entity));
          to_map_func = cType.func("to_map"_hs);
          if (to_map_func) {
            if (ImGui::TreeNode((selected_scene_manager_tabkey + cName +
                                 settings_manager->active_scene)
                                  .c_str(),
                                cName.c_str())) {
              data_ = to_map_func.invoke(cData);
              if (data_) {
                info = data_.cast<
                  std::map<std::string, std::string, NumericComparator>>();
                if (info.empty()) {
                  ImGui::Text("no data");
                } else {
                  draw_leaf_info(info, settings_manager->active_scene);
                }
              } else {
                ENGINE_ERROR("Failed to get to_map for component {0}", cName);
                ImGui::Text("No to_map method defined");
              }
              ImGui::TreePop();
            }
          } else {
            if (ImGui::TreeNode((selected_scene_manager_tabkey + cName +
                                 settings_manager->active_scene)
                                  .c_str(),
                                cName.c_str())) {
              ENGINE_ERROR("Failed to get to_map for component {0}", cName);
              ImGui::Text("No to_map method defined");
              ImGui::TreePop();
            }
          }
        }
      }
    }
  }
  ImGui::Columns(1);
}
}