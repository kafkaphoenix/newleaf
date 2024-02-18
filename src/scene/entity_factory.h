#pragma once

#include <entt/entt.hpp>

#include "assets/assets_manager.h"
#include "assets/prefab.h"
#include "pch.h"
#include "utils/numeric_comparator.h"

namespace potatoengine {

class EntityFactory {
  public:
    using Prototypes = std::map<std::string, entt::entity, NumericComparator>;

    void create_prototypes(
      std::string_view prefab_name,
      const std::vector<std::string>& prototype_ids, entt::registry& registry,
      const std::unique_ptr<assets::AssetsManager>& assets_manager);
    void update_prototypes(
      std::string_view prefab_name,
      const std::vector<std::string>& prototype_ids, entt::registry& registry,
      const std::unique_ptr<assets::AssetsManager>& assets_manager);
    void remove_prototypes(std::string_view prefab_name,
                           const std::vector<std::string>& prototype_ids,
                           entt::registry& registry);
    Prototypes get_prototypes(std::string_view prefab_name,
                              const std::vector<std::string>& prototype_ids);
    bool
    contains_prototypes(std::string_view prefab_name,
                        const std::vector<std::string>& prototype_ids) const;

    const std::map<std::string, Prototypes, NumericComparator>&
    get_all_prototypes();
    const std::map<std::string, std::string, NumericComparator>&
    get_prototypes_count_by_prefab();
    // does not remove entt entities, just clears the map
    void clear_prototypes();

  private:
    std::map<std::string, Prototypes, NumericComparator> m_prefabs;
    std::map<std::string, std::string, NumericComparator>
      m_prototypes_count_by_prefab;
    bool m_dirty{};
};
}