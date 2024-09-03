#pragma once

#include <nlohmann/json.hpp>

#include "../assets/asset.h"
#include "../pch.h"
#include "../utils/numeric_comparator.h"

using json = nlohmann::json;

namespace nl::assets {
class Scene : public Asset {
  public:
    Scene(std::filesystem::path&& fp);

    virtual const std::map<std::string, std::string, NumericComparator>&
    to_map() override final;

    const std::unordered_map<std::string, json>& get_shader_programs() const {
      return m_shaders;
    }
    const std::unordered_map<std::string, json>& get_textures() const {
      return m_textures;
    }
    const std::unordered_map<std::string, json>& get_models() const {
      return m_models;
    }
    const std::unordered_map<std::string, json>& get_prefabs() const {
      return m_prefabs;
    }

    virtual bool operator==(const Asset& other) const override final;

  private:
    std::string m_path;

    std::unordered_map<std::string, json> m_shaders;
    std::unordered_map<std::string, json> m_textures;
    std::unordered_map<std::string, json> m_models;
    std::unordered_map<std::string, json> m_prefabs;

    std::map<std::string, std::string, NumericComparator> m_info;

    void read(const json& j);
};
}