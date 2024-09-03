#pragma once

#include "../pch.h"
#include "../utils/numeric_comparator.h"

#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace nl {

inline std::string
map_to_json(const std::map<std::string, std::string, NumericComparator>& m) {
  json j;
  for (const auto& [key, value] : m) {
    j[key] = value;
  }
  return j.dump();
}

inline std::map<std::string, std::string, NumericComparator>
json_to_map(const std::string& j) {
  json j_ = json::parse(j);
  std::map<std::string, std::string, NumericComparator> m;
  for (const auto& [k, v] : j_.items()) {
    m[k] = v;
  }
  return m;
}
}