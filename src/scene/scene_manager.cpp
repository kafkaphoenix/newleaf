#include "scene/scene_manager.h"

#include "scene/components/core/cName.h"
#include "scene/components/core/cUUID.h"
#include "scene/utils.h"

using namespace entt::literals;

namespace potatoengine {

SceneManager::SceneManager() : m_scene_factory() {
  ENGINE_TRACE("Initializing scene manager...");
  ENGINE_TRACE("Registering engine components...");
  register_components();
  ENGINE_TRACE("Scene manager created!");
}

void SceneManager::register_system(std::string&& name,
                                   std::unique_ptr<systems::System>&& system) {
  ENGINE_ASSERT(not contains_system(name), "System {} already registered",
                name);
  system->init(m_registry);
  m_systems.emplace(std::make_pair(std::move(name), std::move(system)));
  m_dirty_systems = true;
}

void SceneManager::unregister_system(std::string_view name) {
  bool deleted = false;
  for (auto it = m_systems.begin(); it != m_systems.end(); ++it) {
    if (it->first == name) {
      m_systems.erase(it);
      deleted = true;
      break;
    }
  }
  ENGINE_ASSERT(deleted, "System {} not found", name);
  m_dirty_systems = true;
}

bool SceneManager::contains_system(std::string_view name) {
  for (const auto& [n, _] : m_systems) {
    if (n == name) {
      return true;
    }
  }
  return false;
}

void SceneManager::clear_system() {
  m_systems.clear();
  m_dirty_systems = false;
}

void SceneManager::on_update(const Time& ts) {
  for (const auto& [_, system] : m_systems) {
    system->update(m_registry, ts);
  }
}

entt::registry& SceneManager::get_registry() { return m_registry; }

entt::entity SceneManager::get_entity(std::string_view name) {
  for (const auto& [e, cName, _] : m_registry.view<CName, CUUID>().each()) {
    if (cName.name == name) {
      return e;
    }
  }
  ENGINE_ASSERT(false, "Entity with name {} not found", name);
}

entt::entity SceneManager::get_entity(UUID& uuid) {
  for (const auto& [e, cUUID] : m_registry.view<CUUID>().each()) {
    if (cUUID.uuid == uuid) {
      return e;
    }
  }
  ENGINE_ASSERT(false, "Entity with UUID {} not found", std::to_string(uuid));
}

const std::vector<std::string>& SceneManager::get_named_systems() {
  if (not m_dirty_systems) {
    return m_named_systems;
  }

  m_named_systems.clear();
  for (const auto& [name, system] : m_systems) {
    m_named_systems.emplace_back(name + " - Priority " +
                                 std::to_string(system->get_priority()));
  }
  m_dirty_systems = false;

  return m_named_systems;
}

template <typename Component>
void SceneManager::on_component_added(entt::entity e, Component& c) {
  ENGINE_ASSERT(false,
                "Unsupported on_component_added method for component type {}",
                entt::type_id<Component>().name());
}

template <typename Component>
void SceneManager::on_component_cloned(entt::entity e, Component& c) {
  ENGINE_ASSERT(false,
                "Unsupported on_component_cloned method for component type {}",
                entt::type_id<Component>().name());
}

std::unique_ptr<SceneManager> SceneManager::Create() {
  return std::make_unique<SceneManager>();
}

entt::entity SceneManager::create_entity(std::string_view prefab_id,
                                         std::string&& prototype_id,
                                         std::string&& name,
                                         std::optional<std::string> tag,
                                         std::optional<uint32_t> uuid) {
  return m_scene_factory.create_entity(prefab_id, std::move(prototype_id),
                                       m_registry, std::move(name), tag, uuid);
}

entt::entity SceneManager::clone_entity(const entt::entity& e) {
  return m_scene_factory.clone_entity(e, UUID(), m_registry);
}

void SceneManager::delete_entity(entt::entity& e) {
  m_scene_factory.delete_entity(e, m_registry);
}

void SceneManager::create_scene(std::string scene_name,
                                std::string scene_path) {
  auto& app = Application::Get();
  m_scene_factory.create_scene(scene_name, scene_path, app.get_assets_manager(),
                               app.get_render_manager(), m_registry);
  PrintScene(m_registry);
}

void SceneManager::reload_scene(bool reload_prototypes) {
  auto& app = Application::Get();
  m_scene_factory.reload_scene(app.get_assets_manager(),
                               app.get_render_manager(), m_registry,
                               reload_prototypes);
}

void SceneManager::clear_scene() {
  m_scene_factory.clear_scene(Application::Get().get_render_manager(),
                              m_registry);
  m_systems.clear();
  m_named_systems.clear();
  m_dirty_systems = false;
}

std::string SceneManager::get_active_scene() const {
  return m_scene_factory.get_active_scene();
}

const std::map<std::string, entt::entity, NumericComparator>&
SceneManager::get_named_entities() {
  return m_scene_factory.get_named_entities(m_registry);
}

const std::map<std::string, std::string, NumericComparator>&
SceneManager::get_metrics() {
  return m_scene_factory.get_metrics(m_registry);
}

void SceneManager::create_prototypes(
  std::string_view prefab_name, const std::vector<std::string>& prototype_ids) {
  m_scene_factory.get_entity_factory().create_prototypes(
    prefab_name, prototype_ids, m_registry,
    Application::Get().get_assets_manager());
}

void SceneManager::update_prototypes(
  std::string_view prefab_name, const std::vector<std::string>& prototype_ids) {
  m_scene_factory.get_entity_factory().update_prototypes(
    prefab_name, prototype_ids, m_registry,
    Application::Get().get_assets_manager());
}

void SceneManager::remove_prototypes(
  std::string_view prefab_name, const std::vector<std::string>& prototype_ids) {
  m_scene_factory.get_entity_factory().remove_prototypes(
    prefab_name, prototype_ids, m_registry);
}

EntityFactory::Prototypes
SceneManager::get_prototypes(std::string_view prefab_name,
                             const std::vector<std::string>& prototype_ids) {
  return m_scene_factory.get_entity_factory().get_prototypes(prefab_name,
                                                             prototype_ids);
}

bool SceneManager::contains_prototypes(
  std::string_view prefab_name, const std::vector<std::string>& prototype_ids) {
  return m_scene_factory.get_entity_factory().contains_prototypes(
    prefab_name, prototype_ids);
}

const std::map<std::string, EntityFactory::Prototypes, NumericComparator>&
SceneManager::get_all_prototypes() {
  return m_scene_factory.get_entity_factory().get_all_prototypes();
}

const std::map<std::string, std::string, NumericComparator>&
SceneManager::get_prototypes_count_by_prefab() {
  return m_scene_factory.get_entity_factory().get_prototypes_count_by_prefab();
}

void SceneManager::clear_prototypes() {
  m_scene_factory.get_entity_factory().clear_prototypes();
}

}