#pragma once

#include "../utils/numeric_comparator.h"
#include "asset.h"

#include <map>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>

namespace nl::assets {
class AssetsManager {
  public:
    template <typename Type, typename... Args>
    void load(std::string_view id, Args&&... args);

    template <typename Type> bool contains(std::string_view id);

    template <typename Type> std::shared_ptr<Type> get(std::string_view id);

    template <typename Type, typename... Args>
    std::shared_ptr<Type> reload(std::string_view id, Args&&... args);

    void clear();

    static std::unique_ptr<assets::AssetsManager> Create();

    const std::unordered_map<
      std::string, std::unordered_map<std::string, std::shared_ptr<Asset>>>&
    get_assets() const;

    const std::map<std::string, std::string, NumericComparator>& get_metrics();

  private:
    std::unordered_map<std::string,
                       std::unordered_map<std::string, std::shared_ptr<Asset>>>
      m_assets;
    std::map<std::string, std::string, NumericComparator> m_metrics;
    bool m_dirty{};
};
}