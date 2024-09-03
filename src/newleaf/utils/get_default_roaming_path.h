#pragma once

#include "../pch.h"

namespace nl {

inline std::filesystem::path
get_default_roaming_path(std::string_view app_name) {
  auto path = std::filesystem::temp_directory_path()
                .parent_path()
                .parent_path()
                .parent_path();

  path /= "Roaming";

  if (!std::filesystem::exists(path)) {
    std::filesystem::create_directories(path);
  }

  path /= app_name.data();

  if (!std::filesystem::exists(path)) {
    std::filesystem::create_directories(path);
  }

  return path;
}
}