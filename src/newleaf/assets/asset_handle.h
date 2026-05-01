#pragma once

#include <cstddef>
#include <memory>

#include "../utils/uuid.h"

namespace nl {

class AssetsManager;

template <typename Type> class AssetHandle {
  public:
    AssetHandle() = default;
    AssetHandle(AssetsManager* manager, UUID uuid) : m_manager(manager), m_uuid(uuid) {}

    [[nodiscard]] std::shared_ptr<Type> get() const;

    [[nodiscard]] bool is_valid() const { return m_manager != nullptr and m_uuid.value() != 0; }
    [[nodiscard]] UUID uuid() const { return m_uuid; }

    bool operator==(const AssetHandle& other) const { return m_manager == other.m_manager and m_uuid == other.m_uuid; }
    bool operator!=(const AssetHandle& other) const { return not(*this == other); }

  private:
    AssetsManager* m_manager{nullptr};
    UUID m_uuid{0};
};

}

namespace std {
template <typename Type> struct hash<nl::AssetHandle<Type>> {
    size_t operator()(const nl::AssetHandle<Type>& handle) const {
      return std::hash<uint64_t>{}(handle.uuid().value());
    }
};
}
