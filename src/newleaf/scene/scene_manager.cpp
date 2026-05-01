#include "scene_manager.h"

#include "../application/application.h"
#include "../components/meta/cName.h"
#include "../components/meta/cUUID.h"
#include "../logging/log_manager.h"
#include "../utils/assert.h"

using namespace entt::literals;

namespace nl {

SceneManager::SceneManager() : m_scene_factory() { ENGINE_TRACE("initializing scene manager"); }

SceneManager::~SceneManager() { ENGINE_TRACE("deleting scene manager"); }

void SceneManager::register_system(std::string&& name, std::unique_ptr<System>&& system) {
  ENGINE_ASSERT(m_systems_by_name.find(name) == m_systems_by_name.end(), "system {} already registered", name);

  system->init(m_registry);
  System* raw = system.get();

  m_systems_by_priority.insert(raw); // not owning, just a pointer
  m_systems_by_name.emplace(std::move(name), std::move(system));

  m_dirty_systems = true;
}

void SceneManager::unregister_system(std::string_view name) {
  auto it = m_systems_by_name.find(std::string(name));
  ENGINE_ASSERT(it != m_systems_by_name.end(), "system {} not found", name);

  System* raw = it->second.get();
  m_systems_by_priority.erase(raw); // not owning, just remove from set
  m_systems_by_name.erase(it);      // owning, will delete the system

  m_dirty_systems = true;
}

bool SceneManager::contains_system(std::string_view name) const {
  return m_systems_by_name.find(std::string(name)) != m_systems_by_name.end();
}

void SceneManager::update_system_priority(std::string_view name, int32_t priority) {
  auto it = m_systems_by_name.find(std::string(name));
  ENGINE_ASSERT(it != m_systems_by_name.end(), "system {} not found", name);

  System* raw = it->second.get();

  m_systems_by_priority.erase(raw); // not owning, just remove from set
  raw->update_priority(priority);
  m_systems_by_priority.insert(raw); // re-insert with new priority

  m_dirty_systems = true;
}

void SceneManager::clear_systems() {
  m_systems_by_priority.clear();
  m_systems_by_name.clear(); // owning, will delete all systems
  m_dirty_systems = false;
}

void SceneManager::on_update(const Time& ts) {
  for (System* sys : m_systems_by_priority) {
    sys->update(m_registry, ts);
  }
}

entt::registry& SceneManager::get_registry() { return m_registry; }

entt::entity SceneManager::get_entity(std::string_view id) {
  for (const auto& [e, cName, _] : m_registry.view<CName, CUUID>().each()) {
    if (cName.id == id) {
      return e;
    }
  }
  ENGINE_WARN("entity with id {} not found", id);
  return entt::null;
}

entt::entity SceneManager::get_entity(const UUID& uuid) {
  for (const auto& [e, cUUID] : m_registry.view<CUUID>().each()) {
    if (cUUID.uuid == uuid.value()) {
      return e;
    }
  }
  ENGINE_WARN("entity with uuid {} not found", std::to_string(uuid.value()));
  return entt::null;
}

const std::vector<std::string>& SceneManager::get_named_systems() {
  if (!m_dirty_systems) {
    return m_named_systems;
  }

  m_named_systems.clear();
  // Iterate by priority
  for (System* sys : m_systems_by_priority) {
    // Find the corresponding name in map (reverse lookup)
    auto it = std::find_if(m_systems_by_name.begin(), m_systems_by_name.end(),
                           [&](const auto& pair) { return pair.second.get() == sys; });
    if (it != m_systems_by_name.end()) {
      m_named_systems.emplace_back(it->first + " - Priority " + std::to_string(sys->get_priority()));
    }
  }

  m_dirty_systems = false;
  return m_named_systems;
}

template <typename Component> void SceneManager::on_component_added(Component& c) {
  ENGINE_ASSERT(false, "unsupported on_component_added method for component type {}",
                entt::type_id<Component>().name());
}

template <typename Component> void SceneManager::on_component_cloned(entt::entity e, Component& c) {
  ENGINE_ASSERT(false, "unsupported on_component_cloned method for component type {}",
                entt::type_id<Component>().name());
}

std::unique_ptr<SceneManager> SceneManager::create() { return std::make_unique<SceneManager>(); }

entt::entity SceneManager::create_entity(std::string_view prefab_id, std::string&& prototype_id, std::string&& name,
                                         std::optional<std::string> tag, std::optional<uint64_t> uuid) {
  return m_scene_factory.create_entity(prefab_id, std::move(prototype_id), m_registry, std::move(name), tag, uuid);
}

entt::entity SceneManager::clone_entity(entt::entity e) {
  return m_scene_factory.clone_entity(e, UUID().value(), m_registry);
}

void SceneManager::delete_entity(entt::entity e) { m_scene_factory.delete_entity(e, m_registry); }

void SceneManager::delete_entity(std::string_view name) { delete_entity(get_entity(name)); }

void SceneManager::delete_entity(const UUID& uuid) { delete_entity(get_entity(uuid)); }

void SceneManager::create_scene(std::string scene_name, std::string scene_path) {
  m_scene_factory.create_scene(scene_name, scene_path, Application::get().get_assets_manager(), m_registry);
  print_scene();
}

void SceneManager::reload_scene(bool reload_prototypes) {
  m_scene_factory.reload_scene(Application::get().get_assets_manager(), m_registry, reload_prototypes);
}

void SceneManager::clear_scene() {
  m_scene_factory.clear_scene(Application::get().get_render_manager(), m_registry);
  clear_systems();
}

void SceneManager::print_scene() {
  auto entities = m_registry.view<CUUID>();
  entt::meta_type cType;
  entt::meta_any cData;
  entt::meta_func print_func;
  std::string_view cName;
  if (entities.empty()) {
    ENGINE_BACKTRACE("===================entities===================");
    ENGINE_BACKTRACE("no entities in scene");
    ENGINE_BACKTRACE("=============================================");
    return;
  }

  ENGINE_BACKTRACE("===================entities===================");
  ENGINE_BACKTRACE("entities in scene: {}", entities.size());
  for (const auto& e : entities) {
    ENGINE_BACKTRACE("entity uuid: {}", entt::to_integral(e));
    for (const auto& [id, storage] : m_registry.storage()) {
      if (storage.contains(e)) {
        cType = entt::resolve(storage.type());
        cData = cType.construct(storage.value(e));
        print_func = cType.func("print"_hs);
        if (print_func) {
          cName = storage.type().name();
          cName = cName.substr(cName.find_last_of(':') + 1);
          ENGINE_BACKTRACE("\t{}", cName);
          print_func.invoke(cData);
        } else {
          cName = storage.type().name();
          cName = cName.substr(cName.find_last_of(':') + 1);
          ENGINE_ERROR("{} has no print function", cName);
          ENGINE_BACKTRACE("\t{} has no print function", cName);
        }
      }
    }
  }
  ENGINE_BACKTRACE("=============================================");
}

std::string SceneManager::get_active_scene() const { return m_scene_factory.get_active_scene(); }

const std::map<std::string, entt::entity, NumericComparator>& SceneManager::get_named_entities() {
  return m_scene_factory.get_named_entities(m_registry);
}

std::map<std::string, std::string, NumericComparator>& SceneManager::compute_metrics() {
  return m_scene_factory.compute_metrics(m_registry);
}

const std::map<std::string, EntityFactory::Prototypes, NumericComparator>& SceneManager::get_all_prototypes() {
  return m_scene_factory.get_all_prototypes();
}

void SceneManager::clear_prototypes() { m_scene_factory.clear_prototypes(); }

}