#pragma once

#include <entt/entt.hpp>
#include <map>
#include <memory>
#include <optional>
#include <set>
#include <string>
#include <string_view>
#include <vector>

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
    void register_system(std::string&& name,
                         std::unique_ptr<systems::System>&& system);
    void unregister_system(std::string_view name);
    bool contains_system(std::string_view name);
    void clear_system();
    void on_update(const Time& ts);
    entt::registry& get_registry();
    entt::entity get_entity(std::string_view name);
    entt::entity get_entity(UUID& uuid);
    const std::vector<std::string>& get_named_systems();

    template <typename Component>
    void on_component_added(entt::entity e, Component& c);
    template <typename Component>
    void on_component_cloned(entt::entity e, Component& c);

    static std::unique_ptr<SceneManager> Create();

    // scene factory methods
    entt::entity create_entity(std::string_view prefab_id,
                               std::string&& prototype_id, std::string&& name,
                               std::optional<std::string> tag = std::nullopt,
                               std::optional<uint32_t> uuid = std::nullopt);
    entt::entity clone_entity(const entt::entity& e);
    void delete_entity(entt::entity& e);

    void create_scene(std::string scene_name, std::string scene_path);
    void reload_scene(bool reload_prototypes);
    void clear_scene();
    void print_scene();
    std::string get_active_scene() const;
    const std::map<std::string, entt::entity, NumericComparator>&
    get_named_entities();
    const std::map<std::string, std::string, NumericComparator>& get_metrics();

    // entity factory methods
    void create_prototypes(std::string_view prefab_name,
                           const std::vector<std::string>& prototype_ids);
    void update_prototypes(std::string_view prefab_name,
                           const std::vector<std::string>& prototype_ids);
    void remove_prototypes(std::string_view prefab_name,
                           const std::vector<std::string>& prototype_ids);
    EntityFactory::Prototypes
    get_prototypes(std::string_view prefab_name,
                   const std::vector<std::string>& prototype_ids);
    bool contains_prototypes(std::string_view prefab_name,
                             const std::vector<std::string>& prototype_ids);
    const std::map<std::string, EntityFactory::Prototypes, NumericComparator>&
    get_all_prototypes();
    const std::map<std::string, std::string, NumericComparator>&
    get_prototypes_count_by_prefab();
    // does not remove entt entities, just clears the map
    void clear_prototypes();

  private:
    entt::registry m_registry;
    SceneFactory m_scene_factory;
    std::set<std::pair<std::string, std::unique_ptr<systems::System>>,
             systems::SystemComparator>
      m_systems;
    std::vector<std::string> m_named_systems;
    bool m_dirty_systems{};
};
}