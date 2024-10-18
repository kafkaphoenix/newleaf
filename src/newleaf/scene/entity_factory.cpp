#include "entity_factory.h"

#include <entt/core/hashed_string.hpp>
#include <entt/meta/meta.hpp>
#define GLM_FORCE_CTOR_INIT
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <nlohmann/json.hpp>

#include "../components/meta/cDeleted.h"
#include "../logging/log_manager.h"
#include "../utils/assert.h"

using json = nlohmann::json;
using namespace entt::literals;

namespace nl {

glm::vec2 json_to_vec2(const json& j) {
  glm::vec2 v{};
  v.x = j.at("x").get<float>();
  v.y = j.at("y").get<float>();
  return v;
}

glm::vec3 json_to_vec3(const json& j, std::string_view type = "") {
  glm::vec3 vec{};
  vec.x = (type == "color") ? j.at("r").get<float>() : j.at("x").get<float>();
  vec.y = (type == "color") ? j.at("g").get<float>() : j.at("y").get<float>();
  vec.z = (type == "color") ? j.at("b").get<float>() : j.at("z").get<float>();
  return vec;
}

glm::vec4 json_to_vec4(const json& j, std::string_view type = "") {
  glm::vec4 v{};
  v.x = (type == "color") ? j.at("r").get<float>() : j.at("x").get<float>();
  v.y = (type == "color") ? j.at("g").get<float>() : j.at("y").get<float>();
  v.z = (type == "color") ? j.at("b").get<float>() : j.at("z").get<float>();
  v.w = (type == "color") ? j.at("a").get<float>() : j.at("w").get<float>();
  return v;
}

glm::quat json_to_quat(const json& j) {
  glm::quat q{glm::identity<glm::quat>()};
  q.x = j.at("x").get<float>();
  q.y = j.at("y").get<float>();
  q.z = j.at("z").get<float>();
  q.w = j.at("w").get<float>();
  return q;
}

void processCTag(entt::entity& e, std::string_view cTag) {
  entt::meta_type cType = entt::resolve(entt::hashed_string{cTag.data()});
  ENGINE_ASSERT(cType, "no component type found for component tag {}", cTag)

  entt::meta_func assignFunc = cType.func("assign"_hs);
  ENGINE_ASSERT(assignFunc, "no assign function found for component tag {}",
                cTag)

  entt::meta_any metaComponent = assignFunc.invoke({}, e);
  entt::meta_func triggerEventFunc = cType.func("on_component_added"_hs);
  if (triggerEventFunc) {
    triggerEventFunc.invoke({}, e, metaComponent);
  }
}

void processComponent(entt::entity& e, const std::string& cPrefab,
                      const json& cValue) {
  entt::meta_type cType = entt::resolve(entt::hashed_string{cPrefab.data()});
  ENGINE_ASSERT(cType, "no component type found for component {}", cPrefab)

  entt::meta_func assignFunc = cType.func("assign"_hs);
  ENGINE_ASSERT(assignFunc, "no assign function found for component {}",
                cPrefab)

  entt::meta_any metaComponent;
  if (cValue.is_string()) {
    metaComponent = assignFunc.invoke({}, e, cValue.get<std::string>());
  } else if (cValue.is_number_integer()) {
    metaComponent = assignFunc.invoke({}, e, cValue.get<int>());
  } else if (cValue.is_number_float()) {
    metaComponent = assignFunc.invoke({}, e, cValue.get<float>());
  } else if (cValue.is_boolean()) {
    metaComponent = assignFunc.invoke({}, e, cValue.get<bool>());
  } else if (cValue.is_object() and cValue.contains("x") and
             cValue.contains("y") and cValue.contains("z")) {
    metaComponent = assignFunc.invoke({}, e, json_to_vec3(cValue));
  } else if (cValue.is_object()) {
    metaComponent = assignFunc.invoke({}, e);
    ENGINE_ASSERT(metaComponent, "no meta component found for component {}",
                  cPrefab)

    for (const auto& [cField, cFieldValue] : cValue.items()) {
      if (cFieldValue.is_string()) {
        metaComponent.set(entt::hashed_string{cField.data()},
                          cFieldValue.get<std::string>());
      } else if (cFieldValue.is_number_integer()) {
        metaComponent.set(entt::hashed_string{cField.data()},
                          cFieldValue.get<int>());
      } else if (cFieldValue.is_number_float()) {
        metaComponent.set(entt::hashed_string{cField.data()},
                          cFieldValue.get<float>());
      } else if (cFieldValue.is_boolean()) {
        metaComponent.set(entt::hashed_string{cField.data()},
                          cFieldValue.get<bool>());
      } else if (cFieldValue.is_array()) {
        std::vector<std::string> paths;
        paths.reserve(cFieldValue.size());
        for (const auto& value : cFieldValue) {
          ENGINE_ASSERT(value.is_string(),
                        "unsupported type {} for component {} field {}",
                        value.type_name(), cPrefab, cField);
          paths.emplace_back(value.get<std::string>());
        }
        metaComponent.set(entt::hashed_string{cField.data()}, std::move(paths));
      } else if (cFieldValue.is_object()) {
        if (cFieldValue.contains("x") and cFieldValue.contains("y") and
            cFieldValue.contains("z") and cFieldValue.contains("w")) {
          if (cField == "rotation") {
            metaComponent.set(entt::hashed_string{cField.data()},
                              json_to_quat(cFieldValue));
          } else {
            metaComponent.set(entt::hashed_string{cField.data()},
                              json_to_vec4(cFieldValue));
          }
        } else if (cFieldValue.contains("x") and cFieldValue.contains("y") and
                   cFieldValue.contains("z")) {
          if (cField == "rotation") {
            metaComponent.set(
              entt::hashed_string{cField.data()},
              glm::quat(glm::radians(json_to_vec3(cFieldValue))));
          } else {
            metaComponent.set(entt::hashed_string{cField.data()},
                              json_to_vec3(cFieldValue));
          }
        } else if (cFieldValue.contains("x") and cFieldValue.contains("y")) {
          metaComponent.set(entt::hashed_string{cField.data()},
                            json_to_vec2(cFieldValue));
        } else if (cFieldValue.contains("r") and cFieldValue.contains("g") and
                   cFieldValue.contains("b") and cFieldValue.contains("a")) {
          metaComponent.set(entt::hashed_string{cField.data()},
                            json_to_vec4(cFieldValue, "color"));
        } else if (cFieldValue.contains("r") and cFieldValue.contains("g") and
                   cFieldValue.contains("b")) {
          metaComponent.set(entt::hashed_string{cField.data()},
                            json_to_vec3(cFieldValue, "color"));
        } else if (cField == "json") { // TODO unused
          ENGINE_ASSERT(false, "json field is not supported for component {}",
                        cPrefab);
          metaComponent.set(entt::hashed_string{cField.data()},
                            std::move(cFieldValue));
        } else {
          ENGINE_ASSERT(false, "unsupported type {} for component {} field {}",
                        cFieldValue.type_name(), cPrefab, cField)
        }
      } else {
        ENGINE_ASSERT(false, "unsupported type {} for component {} field {}",
                      cFieldValue.type_name(), cPrefab, cField)
      }
    }
  } else {
    ENGINE_ASSERT(false, "unsupported type {} for component {}",
                  cValue.type_name(), cPrefab)
  }
  entt::meta_func triggerEventFunc = cType.func("on_component_added"_hs);
  if (triggerEventFunc) {
    triggerEventFunc.invoke({}, e, metaComponent);
  }
}

void EntityFactory::create_prototypes(
  std::string_view prefab_name, const std::vector<std::string>& prototype_ids,
  entt::registry& registry, const AssetsManager& assets_manager) {
  const auto& prefab = assets_manager.get<Prefab>(prefab_name);

  auto& prefabPrototypes = m_prefabs[prefab_name.data()];
  for (std::string_view prototype_id : prototype_ids) {
    ENGINE_ASSERT(not prefabPrototypes.contains(prototype_id.data()),
                  "prototype {} for prefab {} already exists", prototype_id,
                  prefab_name);
    entt::entity e = registry.create();

    for (std::string_view cTag : prefab->get_ctags(prototype_id)) {
      processCTag(e, cTag);
    }
    for (const auto& [cPrefab, cValue] : prefab->get_components(prototype_id)) {
      processComponent(e, cPrefab, cValue);
    }

    prefabPrototypes.insert({prototype_id.data(), e});
  }
  m_dirty = true;
}

void EntityFactory::update_prototypes(
  std::string_view prefab_name, const std::vector<std::string>& prototype_ids,
  entt::registry& registry, const AssetsManager& assets_manager) {
  delete_prototypes(prefab_name, prototype_ids, registry);
  create_prototypes(prefab_name, prototype_ids, registry, assets_manager);
}

void EntityFactory::delete_prototypes(
  std::string_view prefab_name, const std::vector<std::string>& prototype_ids,
  entt::registry& registry) {
  for (std::string_view prototype_id : prototype_ids) {
    ENGINE_ASSERT(
      m_prefabs.at(prefab_name.data()).contains(prototype_id.data()),
      "unknown prototype {} for prefab {}", prototype_id, prefab_name);
    registry.emplace<CDeleted>(
      m_prefabs.at(prefab_name.data()).at(prototype_id.data()));
    m_prefabs.at(prefab_name.data()).erase(prototype_id.data());
  }
  m_dirty = true;
}

EntityFactory::Prototypes
EntityFactory::get_prototypes(std::string_view prefab_name,
                              const std::vector<std::string>& prototype_ids) {
  ENGINE_ASSERT(m_prefabs.contains(prefab_name.data()), "unknown prefab {}",
                prefab_name);
  Prototypes prototypes;
  for (std::string_view prototype_id : prototype_ids) {
    ENGINE_ASSERT(
      m_prefabs.at(prefab_name.data()).contains(prototype_id.data()),
      "unknown prototype {} for prefab {}", prototype_id, prefab_name);
    prototypes.insert(
      {prototype_id.data(),
       m_prefabs.at(prefab_name.data()).at(prototype_id.data())});
  }
  return prototypes;
}

bool EntityFactory::contains_prototypes(
  std::string_view prefab_name,
  const std::vector<std::string>& prototype_ids) const {
  ENGINE_ASSERT(m_prefabs.contains(prefab_name.data()), "unknown prefab {}",
                prefab_name);
  for (std::string_view prototype_id : prototype_ids) {
    if (not m_prefabs.at(prefab_name.data()).contains(prototype_id.data())) {
      return false;
    }
  }
  return true;
}

const std::map<std::string, std::string, NumericComparator>&
EntityFactory::get_prototypes_count_by_prefab() {
  if (not m_dirty) {
    return m_prototypes_count_by_prefab;
  }

  m_prototypes_count_by_prefab.clear();
  for (const auto& [prefab_name, prototypes] : m_prefabs) {
    m_prototypes_count_by_prefab["prefab_" + prefab_name] =
      std::to_string(prototypes.size());
  }
  m_dirty = false;

  return m_prototypes_count_by_prefab;
}

const std::map<std::string, EntityFactory::Prototypes, NumericComparator>&
EntityFactory::get_all_prototypes() {
  return m_prefabs;
}

void EntityFactory::clear_prototypes() {
  m_prefabs.clear();
  m_prototypes_count_by_prefab.clear();
  m_dirty = false;
}

}