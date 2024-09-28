#include "assets_manager.h"

#include "../logging/log_manager.h"
#include "../utils/assert.h"

namespace nl {

void AssetsManager::clear() {
  m_assets.clear();
  m_metrics.clear();
  m_dirty = false;
}

std::unique_ptr<AssetsManager> AssetsManager::create() {
  return std::make_unique<AssetsManager>();
}

const std::unordered_map<
  std::string, std::unordered_map<std::string, std::shared_ptr<Asset>>>&
AssetsManager::get_assets() const {
  return m_assets;
}

std::map<std::string, std::string, NumericComparator>&
AssetsManager::compute_metrics() {
  if (not m_dirty) {
    return m_metrics;
  }

  m_metrics.clear();
  for (const auto& [key, value] : m_assets) {
    m_metrics.emplace(key, std::to_string(value.size()));
  }
  m_dirty = false;

  return m_metrics;
}
}