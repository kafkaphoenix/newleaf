#pragma once

#include <imgui.h>
#include <map>
#include <memory>
#include <string>

#include "../settings/settings_manager.h"
#include "../state/states_manager.h"
#include "../utils/map_json_serializer.h"
#include "../utils/numeric_comparator.h"
#include "im_utils.h"

namespace nl {

char states_text_filter[128]{}; // TODO: move to class
bool filter_states{};
bool filter_layers{};

inline void
draw_states_manager(const std::unique_ptr<SettingsManager>& settings_manager,
                    const std::unique_ptr<StatesManager>& states_manager) {
  int collapsed = collapser();

  ImGui::InputText("##filter", states_text_filter,
                   IM_ARRAYSIZE(states_text_filter));
  if (ImGui::IsItemHovered()) {
    ImGui::SetTooltip("Filter states or layers by name");
  }
  ImGui::SameLine();
  if (ImGui::Button("Clear Filter")) {
    states_text_filter[0] = '\0';
  }
  ImGui::Checkbox("States", &filter_states);
  ImGui::SameLine();
  ImGui::Checkbox("Layers", &filter_layers);

  ImGui::Separator();

  if (collapsed not_eq -1) {
    ImGui::SetNextItemOpen(collapsed not_eq 0);
  }

  if (states_manager->get_current_state() == nullptr) {
    ImGui::Text("No states");
  }

  for (const auto& [state, layers] : states_manager->get_metrics()) {
    if (filter_states and states_text_filter[0] not_eq '\0' and
        strstr(state.c_str(), states_text_filter) == nullptr) {
      continue;
    }
    if (ImGui::CollapsingHeader(state.c_str())) {
      std::map<std::string, std::string, NumericComparator> layersMap =
        json_to_map(layers);
      for (const auto& [name, enabled] : layersMap) {
        if (filter_layers and states_text_filter[0] not_eq '\0' and
            strstr(name.c_str(), states_text_filter) == nullptr) {
          continue;
        }
        ImGui::BulletText("%s - %s", name.c_str(), enabled.c_str());
      }
    }
  }
}
}
