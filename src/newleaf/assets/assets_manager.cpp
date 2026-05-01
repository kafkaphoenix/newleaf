#include "assets_manager.h"

#include "../logging/log_manager.h"

namespace nl {

AssetsManager::AssetsManager() { ENGINE_TRACE("initializing assets manager"); }

AssetsManager::~AssetsManager() { ENGINE_TRACE("deleting assets manager"); }

void AssetsManager::clear() {
  ENGINE_TRACE("clearing assets manager");
  m_assets.clear();
  m_index.clear();
  m_view.clear();
  m_metrics.clear();
  m_dirty_view = false;
  m_dirty_metrics = false;
}

std::unique_ptr<AssetsManager> AssetsManager::create() { return std::make_unique<AssetsManager>(); }

const std::unordered_map<std::string, std::unordered_map<std::string, std::shared_ptr<Asset>>>&
AssetsManager::get_assets() const {
  if (not m_dirty_view)
    return m_view;

  m_view.clear();
  m_view.reserve(m_index.size());
  for (const auto& [type, ids] : m_index) {
    auto& bucket = m_view[type];
    bucket.reserve(ids.size());
    for (const auto& [id, uuid] : ids)
      bucket.emplace(id, m_assets.at(uuid));
  }

  m_dirty_view = false;
  return m_view;
}

std::map<std::string, std::string, NumericComparator>& AssetsManager::compute_metrics() {
  if (not m_dirty_metrics)
    return m_metrics;

  m_metrics.clear();
  for (const auto& [type, ids] : m_index)
    m_metrics.emplace(type, std::to_string(ids.size()));
  m_dirty_metrics = false;

  return m_metrics;
}

}