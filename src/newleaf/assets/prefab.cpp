#include "prefab.h"

#include <fstream>

#include "../core/log_manager.h"
#include "../utils/timer.h"

namespace nl::assets {

void Prefab::process_prototype(const std::string& name,
                               const json& prototype_data, const json& data) {
  std::vector<std::string> inherits;
  std::vector<std::string> ctags;
  std::unordered_map<std::string, json> components;

  if (prototype_data.contains("inherits")) {
    prototype_data.at("inherits").get_to(inherits);
    for (std::string_view father : inherits) {
      read(data.at(father), inherits, ctags, components);
    }
  }
  read(prototype_data, inherits, ctags,
       components); // child overrides parent if common definition exists

  m_prototypes.emplace(name, Prototype{.inherits = std::move(inherits),
                                       .ctags = std::move(ctags),
                                       .components = std::move(components)});
}

Prefab::Prefab(std::filesystem::path&& fp,
               std::vector<std::string>&& targetedPrototypes)
  : m_name(std::move(fp.filename().string())), m_path(std::move(fp.string())),
    m_targeted_prototypes(std::move(targetedPrototypes)) {
  // One prefab file can contain multiple prototypes and we target only a subset
  // of them
  std::ifstream f(fp);
  ENGINE_ASSERT(f.is_open(), "failed to open prefab file!");
  ENGINE_ASSERT(f.peek() not_eq std::ifstream::traits_type::eof(),
                "prefab file is empty!");
  json data = json::parse(f);
  f.close();

  if (m_targeted_prototypes == std::vector<std::string>{"*"}) {
    m_targeted_prototypes.clear();
    for (const auto& [name, prototype_data] : data.items()) {
      m_targeted_prototypes.emplace_back(name);
      process_prototype(name, prototype_data, data);
    }
  } else {
    for (const auto& [name, prototype_data] : data.items()) {
      if (std::find(m_targeted_prototypes.begin(), m_targeted_prototypes.end(),
                    name) == m_targeted_prototypes.end()) {
        continue;
      }
      process_prototype(name, prototype_data, data);
    }
  }
}

void Prefab::read(const json& data, std::vector<std::string>& inherits,
                  std::vector<std::string>& ctags,
                  std::unordered_map<std::string, json>& components) {
  if (data.contains("ctags")) {
    for (const json& c : data.at("ctags")) {
      ctags.emplace_back(c);
    }
  }

  if (data.contains("components")) {
    for (const auto& [cKey, cValue] : data.at("components").items()) {
      if (inherits.size() > 0) {
        std::erase_if(ctags,
                      [&cKey](const std::string& c) { return c == cKey; });
      }
      if (components.contains(cKey)) {
        for (const auto& [cFieldKey, cFieldValue] : cValue.items()) {
          if (not cFieldKey.empty()) {
            components[cKey][cFieldKey] = cFieldValue;
          } else {
            components[cKey] = cFieldValue;
          }
        }
      } else {
        components[cKey] = cValue;
      }
    }
  }
}

const std::map<std::string, std::string, NumericComparator>& Prefab::to_map() {
  if (not m_info.empty()) {
    return m_info;
  }

  m_info["type"] = "prefab";
  m_info["path"] = m_path;
  for (const auto& [prototype_id, prototype_data] : m_prototypes) {
    m_info["prototype_" + prototype_id] = prototype_id;
  }

  return m_info;
}

const std::map<std::string, std::string, NumericComparator>&
Prefab::get_targeted_prototype_info(std::string_view prototype_id) {
  if (not m_prototype_info.empty() and
      m_prototype_info.contains(prototype_id.data())) {
    return m_prototype_info.at(prototype_id.data());
  }

  std::map<std::string, std::string, NumericComparator> m_info{};
  m_info["name"] = prototype_id.data();
  for (uint32_t i = 0; i < m_prototypes.at(prototype_id.data()).inherits.size();
       ++i) {
    m_info["inherits " + std::to_string(i)] =
      *std::next(m_prototypes.at(prototype_id.data()).inherits.begin(), i);
  }
  for (uint32_t i = 0; i < m_prototypes.at(prototype_id.data()).ctags.size();
       ++i) {
    m_info["cTag " + std::to_string(i)] =
      *std::next(m_prototypes.at(prototype_id.data()).ctags.begin(), i);
  }
  uint32_t i = 0;
  for (const auto& [componentID, _] :
       m_prototypes.at(prototype_id.data()).components) {
    m_info["component " + std::to_string(i++)] = componentID;
  }
  m_prototype_info[prototype_id.data()] = m_info;

  return m_prototype_info.at(prototype_id.data());
}

bool Prefab::operator==(const Asset& other) const {
  if (typeid(*this) not_eq typeid(other)) {
    ENGINE_ASSERT(false, "cannot compare prefab with other asset type!");
  }
  const Prefab& otherPrefab = static_cast<const Prefab&>(other);
  return m_path == otherPrefab.m_path;
}
}