#pragma once

#include <map>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>

#include "../logging/log_manager.h"
#include "../utils/assert.h"
#include "../utils/numeric_comparator.h"
#include "../utils/uuid.h"
#include "asset.h"
#include "asset_handle.h"

namespace nl {
class AssetsManager {
  public:
    AssetsManager();
    ~AssetsManager();

    template <typename Type, typename... Args> AssetHandle<Type> get_or_load(std::string_view id, Args&&... args) {
      const std::string type = get_type_name<Type>();
      auto& ids = m_assets_by_type[type]; // create type map if not exists
      auto it = ids.find(id.data());
      if (it != ids.end()) {
        return AssetHandle<Type>(this, it->second);
      }
      UUID uuid;
      m_assets_by_uuid.emplace(uuid, std::make_shared<Type>(std::forward<Args>(args)...));
      ids.emplace(id, uuid);
      m_dirty_view = true;
      m_dirty_metrics = true;
      return AssetHandle<Type>(this, uuid);
    }

    template <typename Type> AssetHandle<Type> get(std::string_view id) const {
      const std::string type = get_type_name<Type>();
      auto it = m_assets_by_type.find(type);
      ENGINE_ASSERT(it != m_assets_by_type.end() && it->second.contains(id.data()), "asset {} not found for type {}!",
                    id, type);
      return AssetHandle<Type>(const_cast<AssetsManager*>(this), it->second.at(id.data()));
    }

    template <typename Type> AssetHandle<Type> try_get(std::string_view id) const {
      const std::string type = get_type_name<Type>();
      auto it = m_assets_by_type.find(type);
      if (it == m_assets_by_type.end()) {
        return AssetHandle<Type>();
      }
      auto id_it = it->second.find(id.data());
      if (id_it == it->second.end()) {
        return AssetHandle<Type>();
      }
      return AssetHandle<Type>(const_cast<AssetsManager*>(this), id_it->second);
    }

    void clear();

    static std::unique_ptr<AssetsManager> create();

    const std::unordered_map<std::string, std::unordered_map<std::string, std::shared_ptr<Asset>>>& get_assets() const;

    std::map<std::string, std::string, NumericComparator>& compute_metrics();

  private:
    template <typename Type> static std::string get_type_name() {
      std::string_view type = typeid(Type).name();
      type = type.substr(type.find_last_of(':') + 1);
      return std::string(type);
    }

    template <typename Type> std::shared_ptr<Type> get_asset_ptr(UUID uuid) const {
      auto it = m_assets_by_uuid.find(uuid);
      if (it == m_assets_by_uuid.end()) {
        return nullptr;
      }
      return std::dynamic_pointer_cast<Type>(it->second);
    }

    std::unordered_map<UUID, std::shared_ptr<Asset>> m_assets_by_uuid;
    std::unordered_map<std::string, std::unordered_map<std::string, UUID>> m_assets_by_type;
    mutable std::unordered_map<std::string, std::unordered_map<std::string, std::shared_ptr<Asset>>> m_assets_view;
    std::map<std::string, std::string, NumericComparator> m_metrics;
    mutable bool m_dirty_view{};
    bool m_dirty_metrics{};

    template <typename Type> friend class AssetHandle;
};
}

template <typename Type> std::shared_ptr<Type> nl::AssetHandle<Type>::get() const {
  if (!is_valid()) {
    return nullptr;
  }
  return m_manager->get_asset_ptr<Type>(m_uuid);
}