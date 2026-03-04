#pragma once

#include <cstddef>
#include <memory>

#include "../utils/uuid.h"

namespace nl {

class AssetsManager;

template <typename Type> class AssetHandle {
  public:
    AssetHandle() : m_manager(nullptr), m_uuid(UUID(0)) {}
    AssetHandle(AssetsManager* manager, UUID uuid) : m_manager(manager), m_uuid(uuid) {}

    std::shared_ptr<Type> get() const;

    bool is_valid() const { return m_manager != nullptr && m_uuid.value() != 0; }
    UUID uuid() const { return m_uuid; }

    bool operator==(const AssetHandle& other) const { return m_manager == other.m_manager && m_uuid == other.m_uuid; }
    bool operator!=(const AssetHandle& other) const { return !(*this == other); }

  private:
    AssetsManager* m_manager;
    UUID m_uuid;
};
}

namespace std {
template <typename Type> struct hash<nl::AssetHandle<Type>> {
    size_t operator()(const nl::AssetHandle<Type>& handle) const {
      return std::hash<uint64_t>{}(handle.uuid().value());
    }
};
}
