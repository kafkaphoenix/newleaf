#pragma once

#include <map>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>

#include "../logging/log_manager.h"
#include "../utils/assert.h"
#include "../utils/numeric_comparator.h"
#include "asset.h"

namespace nl {
class AssetsManager {
  public:
    template <typename Type, typename... Args>
    void load(std::string_view id, Args&&... args) {
      std::string_view type = typeid(Type).name();
      type = type.substr(type.find_last_of(':') + 1);
      auto& asset_map = m_assets[type.data()];
      ENGINE_ASSERT(not asset_map.contains(id.data()),
                    "asset {} already exists for type {}!", id, type);
      asset_map.emplace(id,
                        std::make_shared<Type>(std::forward<Args>(args)...));
      m_dirty = true;
    }

    template <typename Type> bool contains(std::string_view id) const {
      std::string_view type = typeid(Type).name();
      type = type.substr(type.find_last_of(':') + 1);
      auto& asset_map = m_assets.at(type.data());
      return asset_map.contains(id.data());
    }

    template <typename Type>
    std::shared_ptr<Type> get(std::string_view id) const {
      std::string_view type = typeid(Type).name();
      type = type.substr(type.find_last_of(':') + 1);
      ENGINE_ASSERT(contains<Type>(id), "asset {} not found for type {}!", id,
                    type);
      return std::static_pointer_cast<Type>(
        m_assets.at(type.data()).at(id.data())); // I know the type is correct
    }

    template <typename Type, typename... Args>
    std::shared_ptr<Type> reload(std::string_view id, Args&&... args) {
      std::string_view type = typeid(Type).name();
      type = type.substr(type.find_last_of(':') + 1);
      ENGINE_ASSERT(contains<Type>(id), "asset {} not found for type {}!", id,
                    type);
      auto& maybe_asset = m_assets.at(type.data()).at(id.data());
      std::shared_ptr<Asset> asset =
        std::make_shared<Type>(std::forward<Args>(args)...);
      maybe_asset = std::move(asset);

      m_dirty = true;
      ENGINE_TRACE("reloaded asset {}", id);
      return std::static_pointer_cast<Type>(
        maybe_asset); // I know the type is correct
    }

    void clear();

    static std::unique_ptr<AssetsManager> create();

    const std::unordered_map<
      std::string, std::unordered_map<std::string, std::shared_ptr<Asset>>>&
    get_assets() const;

    std::map<std::string, std::string, NumericComparator>& compute_metrics();

  private:
    std::unordered_map<std::string,
                       std::unordered_map<std::string, std::shared_ptr<Asset>>>
      m_assets;
    std::map<std::string, std::string, NumericComparator> m_metrics;
    bool m_dirty{};
};
}