#pragma once

#include <map>
#include <memory>
#include <optional>
#include <string>
#include <string_view>

#include <entt/entt.hpp>

#include "../assets/assets_manager.h"
#include "../assets/scene.h"
#include "../graphics/render_manager.h"
#include "../utils/numeric_comparator.h"
#include "entity_factory.h"

namespace nl {

class SceneFactory {
  public:
    SceneFactory();
    entt::entity create_entity(std::string_view prefab_id,
                               std::string&& prototype_id,
                               entt::registry& registry, std::string&& name,
                               std::optional<std::string> tag = std::nullopt,
                               std::optional<uint32_t> uuid = std::nullopt);
    entt::entity clone_entity(const entt::entity& e, uint32_t uuid,
                              entt::registry& registry,
                              std::optional<std::string> name = std::nullopt,
                              std::optional<std::string> tag = std::nullopt);
    void delete_entity(entt::entity& e, entt::registry& registry);

    void create_scene(std::string scene_name, std::string scene_path,
                      AssetsManager& assets_manager,
                      RenderManager& render_manager, entt::registry& registry);
    void reload_scene(const AssetsManager& assets_manager,
                      const RenderManager& render_manager,
                      entt::registry& registry, bool reload_prototypes);

    void clear_scene(RenderManager& render_manager, entt::registry& registry);

    std::string get_active_scene() const { return m_active_scene; }
    std::map<std::string, std::string, NumericComparator>&
    compute_metrics(entt::registry& registry);
    const std::map<std::string, entt::entity, NumericComparator>&
    get_named_entities(entt::registry& registry);

    EntityFactory& get_entity_factory() { return m_entity_factory; }
    const EntityFactory& get_entity_factory() const { return m_entity_factory; }

  private:
    std::string m_active_scene;
    EntityFactory m_entity_factory;

    std::map<std::string, std::string, NumericComparator> m_metrics;
    std::map<std::string, entt::entity, NumericComparator> m_named_entities;
    bool m_dirty_metrics{};
    bool m_dirty_named_entities{};

    void create_shader_programs(const Scene& scene,
                                AssetsManager& assets_manager,
                                RenderManager& render_manager);
    void create_textures(const Scene& scene, AssetsManager& assets_manager);
    void create_models(const Scene& scene, AssetsManager& assets_manager);
    void create_prototypes(const Scene& scene, AssetsManager& assets_manager,
                           entt::registry& registry);
};
}