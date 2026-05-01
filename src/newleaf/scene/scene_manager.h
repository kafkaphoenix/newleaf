#pragma once

#include <map>
#include <memory>
#include <optional>
#include <set>
#include <string>
#include <string_view>
#include <vector>

#include <entt/entt.hpp>

#include "../assets/assets_manager.h"
#include "../events/event.h"
#include "../systems/system.h"
#include "../utils/numeric_comparator.h"
#include "../utils/time.h"
#include "../utils/uuid.h"
#include "scene_factory.h"

namespace nl {
class SceneManager {
  public:
    SceneManager();
    ~SceneManager();
    void register_system(std::string&& name, std::unique_ptr<System>&& system);
    void unregister_system(std::string_view name);
    bool contains_system(std::string_view name) const;
    void update_system_priority(std::string_view name, int32_t priority);
    void clear_systems();
    void on_update(const Time& ts);
    entt::registry& get_registry();
    entt::entity get_entity(std::string_view name);
    entt::entity get_entity(const UUID& uuid);
    const std::vector<std::string>& get_named_systems();

    template <typename Component> void on_component_added(Component& c);
    template <typename Component> void on_component_cloned(entt::entity e, Component& c);

    static std::unique_ptr<SceneManager> create();

    // scene factory methods
    entt::entity create_entity(std::string_view prefab_id, std::string&& prototype_id, std::string&& name,
                               std::optional<std::string> tag = std::nullopt,
                               std::optional<uint64_t> uuid = std::nullopt);
    entt::entity clone_entity(entt::entity e);
    void delete_entity(entt::entity e);
    void delete_entity(std::string_view name);
    void delete_entity(const UUID& uuid);

    void create_scene(std::string scene_name, std::string scene_path);
    void reload_scene(bool reload_prototypes);
    void clear_scene();
    void print_scene();
    std::string get_active_scene() const;
    const std::map<std::string, entt::entity, NumericComparator>& get_named_entities();
    std::map<std::string, std::string, NumericComparator>& compute_metrics();

    const std::map<std::string, EntityFactory::Prototypes, NumericComparator>& get_all_prototypes();
    // does not delete entt entities, just clears the map
    void clear_prototypes();

  private:
    entt::registry m_registry;
    SceneFactory m_scene_factory;
    std::map<std::string, std::unique_ptr<System>> m_systems_by_name;  // fast lookup
    std::set<System*, SystemPriorityComparator> m_systems_by_priority; // iteration by priority no ownership
    std::vector<std::string> m_named_systems;
    bool m_dirty_systems{};
};
}