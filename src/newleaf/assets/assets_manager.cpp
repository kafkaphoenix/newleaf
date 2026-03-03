#include "assets_manager.h"

#include "../logging/log_manager.h"
#include "../utils/assert.h"

namespace nl {

AssetsManager::AssetsManager() { ENGINE_TRACE("initializing assets manager"); }

AssetsManager::~AssetsManager() { ENGINE_WARN("deleting assets manager"); }

void AssetsManager::clear() {
  ENGINE_DEBUG("clearing assets manager");
  m_assets_by_uuid.clear();
  m_assets_by_type.clear();
  m_assets_view.clear();
  m_metrics.clear();
  m_dirty_view = false;
  m_dirty_metrics = false;
}

std::unique_ptr<AssetsManager> AssetsManager::create() { return std::make_unique<AssetsManager>(); }

const std::unordered_map<std::string, std::unordered_map<std::string, std::shared_ptr<Asset>>>&
AssetsManager::get_assets() const {
  if (not m_dirty_view) {
    return m_assets_view;
  }

  m_assets_view.clear();
  for (const auto& [type, ids] : m_assets_by_type) {
    auto& bucket = m_assets_view[type];
    for (const auto& [id, uuid] : ids) {
      bucket.emplace(id, m_assets_by_uuid.at(uuid));
    }
  }

  m_dirty_view = false;
  return m_assets_view;
}

std::map<std::string, std::string, NumericComparator>& AssetsManager::compute_metrics() {
  if (not m_dirty_metrics) {
    return m_metrics;
  }

  m_metrics.clear();
  for (const auto& [key, value] : m_assets_by_type) {
    m_metrics.emplace(key, std::to_string(value.size()));
  }
  m_dirty_metrics = false;

  return m_metrics;
}
}