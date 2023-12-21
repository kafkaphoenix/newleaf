#pragma once

#include <nlohmann/json.hpp>

#include "pch.h"
#include "utils/numericComparator.h"

using json = nlohmann::json;

namespace potatoengine {
class Scene {
  public:
    Scene(std::filesystem::path&& fp);
    Scene& operator=(const Scene&) = delete;

    const std::map<std::string, std::string, NumericComparator>& getInfo();

    const std::unordered_map<std::string, json>& getShaders() const noexcept {
      return m_shaders;
    }
    const std::unordered_map<std::string, json>& getTextures() const noexcept {
      return m_textures;
    }
    const std::unordered_map<std::string, json>& getModels() const noexcept {
      return m_models;
    }
    const std::unordered_map<std::string, json>& getPrefabs() const noexcept {
      return m_prefabs;
    }
    const std::unordered_map<std::string, json>& getScenes() const noexcept {
      return m_scenes;
    }

    const std::unordered_map<std::string, json>&
    getNormalEntities() const noexcept {
      return m_normalEntities;
    }
    const std::unordered_map<std::string, json>&
    getLightEntities() const noexcept {
      return m_lightEntities;
    }
    const std::unordered_map<std::string, json>&
    getCameraEntities() const noexcept {
      return m_cameraEntities;
    }
    const std::unordered_map<std::string, json>&
    getSystemEntities() const noexcept {
      return m_systemEntities;
    }
    const std::unordered_map<std::string, json>&
    getFBOEntities() const noexcept {
      return m_fboEntities;
    }

    bool operator==(const Scene& other) const noexcept {
      return m_filepath == other.m_filepath;
    }

  private:
    std::string m_filepath{};

    std::unordered_map<std::string, json> m_shaders{};
    std::unordered_map<std::string, json> m_textures{};
    std::unordered_map<std::string, json> m_models{};
    std::unordered_map<std::string, json> m_prefabs{};
    std::unordered_map<std::string, json> m_scenes{};

    std::unordered_map<std::string, json> m_normalEntities{};
    std::unordered_map<std::string, json> m_lightEntities{};
    std::unordered_map<std::string, json> m_cameraEntities{};
    std::unordered_map<std::string, json> m_systemEntities{};
    std::unordered_map<std::string, json> m_fboEntities{};

    std::map<std::string, std::string, NumericComparator> m_info{};

    void read(const json& j);
};
}