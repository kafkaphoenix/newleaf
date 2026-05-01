#pragma once

#include <functional>
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
#include "stringhash.h"

namespace nl {

class AssetsManager {
  public:
    AssetsManager();
    ~AssetsManager();

    template <typename Type, typename... Args> AssetHandle<Type> load(std::string_view uuid, Args&&... args) {
      ENGINE_ASSERT(!uuid.empty(), "asset uuid cannot be empty for type {}!", get_type_name<Type>());
      ENGINE_ASSERT(!contains<Type>(uuid), "asset {} already exists for type {}!", uuid, get_type_name<Type>());

      UUID index;
      auto asset = std::make_shared<Type>(std::forward<Args>(args)...);
      asset->set_uuid(std::string(uuid));
      m_assets.emplace(index, std::move(asset));
      m_index[get_type_name<Type>()].emplace(std::string(uuid), index);
      m_dirty_view = true;
      m_dirty_metrics = true;
      return AssetHandle<Type>(this, index);
    }

    template <typename Type, typename... Args> AssetHandle<Type> get_or_load(std::string_view uuid, Args&&... args) {
      if (contains<Type>(uuid))
        return get<Type>(uuid);
      return load<Type>(uuid, std::forward<Args>(args)...);
    }

    template <typename Type> AssetHandle<Type> get(std::string_view uuid) const {
      const std::string type = get_type_name<Type>();
      auto type_it = m_index.find(type);
      ENGINE_ASSERT(type_it != m_index.end(), "no assets of type {} exist!", type);
      auto uuid_it = type_it->second.find(uuid);
      ENGINE_ASSERT(uuid_it != type_it->second.end(), "asset {} not found for type {}!", uuid, type);
      return AssetHandle<Type>(const_cast<AssetsManager*>(this), uuid_it->second);
    }

    template <typename Type> void unload(std::string_view id) {
      const std::string type = get_type_name<Type>();
      auto type_it = m_index.find(type);
      ENGINE_ASSERT(type_it != m_index.end(), "no assets of type {} exist!", type);
      auto id_it = type_it->second.find(id);
      ENGINE_ASSERT(id_it != type_it->second.end(), "asset {} not found for type {}!", id, type);
      m_assets.erase(id_it->second);
      type_it->second.erase(id_it);
      if (type_it->second.empty())
        m_index.erase(type_it);
      m_dirty_view = true;
      m_dirty_metrics = true;
    }

    template <typename Type> bool contains(std::string_view id) const {
      auto type_it = m_index.find(get_type_name<Type>());
      return type_it != m_index.end() and type_it->second.contains(id);
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
      auto it = m_assets.find(uuid);
      if (it == m_assets.end())
        return nullptr;
      return std::static_pointer_cast<Type>(it->second);
    }

    std::unordered_map<UUID, std::shared_ptr<Asset>> m_assets;
    std::unordered_map<std::string, std::unordered_map<std::string, UUID, StringHash, std::equal_to<>>> m_index;
    mutable std::unordered_map<std::string, std::unordered_map<std::string, std::shared_ptr<Asset>>> m_view;
    std::map<std::string, std::string, NumericComparator> m_metrics;
    mutable bool m_dirty_view{};
    bool m_dirty_metrics{};

    template <typename Type> friend class AssetHandle;
};

}

template <typename Type> std::shared_ptr<Type> nl::AssetHandle<Type>::get() const {
  if (not is_valid() or m_manager == nullptr)
    return nullptr;
  return m_manager->get_asset_ptr<Type>(m_uuid);
}