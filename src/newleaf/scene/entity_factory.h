#pragma once

#include <map>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include <entt/entt.hpp>

#include "../assets/asset_handle.h"
#include "../assets/prefab.h"
#include "../utils/numeric_comparator.h"

namespace nl {

class EntityFactory {
  public:
    using Prototypes = std::map<std::string, entt::entity, NumericComparator>;

    void create_prototypes(AssetHandle<Prefab> handle, entt::registry& registry);
    void update_prototypes(AssetHandle<Prefab> handle, entt::registry& registry);
    void delete_prototypes(AssetHandle<Prefab> handle, entt::registry& registry);
    Prototypes get_prototypes(std::string_view uuid, const std::vector<std::string>& prototype_ids);

    const std::map<std::string, Prototypes, NumericComparator>& get_all_prototypes();
    // does not delete entt entities, just clears the map
    void clear_prototypes();

  private:
    std::map<std::string, Prototypes, NumericComparator> m_prefabs;
    std::map<std::string, std::string, NumericComparator> m_prototypes_count_by_prefab;
    bool m_dirty{};
};
}