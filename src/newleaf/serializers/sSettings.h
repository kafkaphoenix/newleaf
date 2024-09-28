#pragma once

#include <filesystem>
#include <format>
#include <fstream>
#include <memory>
#include <string_view>

#include <nlohmann/json.hpp>

#include "../settings/settings_manager.h"
#include "../utils/assert.h"
#include "../utils/get_default_roaming_path.h"

using json = nlohmann::json;

namespace nl {

inline void save_settings(const SettingsManager& settings_manager,
                          std::filesystem::path path) {
  json data = *settings_manager;

  if (path.filename() not_eq "settings.json") {
    path /= "settings.json";
  }

  std::ofstream file(path);
  ENGINE_ASSERT(file.is_open(), "failed to open settings file!");

  file << data.dump(4);
  file.close();
}

inline std::unique_ptr<SettingsManager>
load_settings(std::string_view app_name) {
  std::unique_ptr<SettingsManager> settings_manager =
    std::make_unique<SettingsManager>();
  auto path = get_default_roaming_path(app_name) / "settings.json";

  if (!std::filesystem::exists(path)) {
    settings_manager.app_name = app_name.data();
    settings_manager.logfile_path = std::format("logs/{}.log", app_name);
    save_settings(settings_manager, path);
  } else {
    std::ifstream file(path);
    ENGINE_ASSERT(file.is_open(), "failed to open settings file!");
    ENGINE_ASSERT(file.peek() not_eq std::ifstream::traits_type::eof(),
                  "settings file is empty!");
    json data = json::parse(file);
    file.close();
    *settings_manager = data.get<SettingsManager>();
  }

  return settings_manager;
}

}