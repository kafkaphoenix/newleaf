#include "scene.h"

#include <fstream>

#include "../logging/log_manager.h"
#include "../utils/assert.h"
#include "../utils/timer.h"

namespace nl::assets {
Scene::Scene(std::filesystem::path&& fp) : m_path(std::move(fp.string())) {
  std::ifstream f(fp);
  ENGINE_ASSERT(f.is_open(), "failed to open scene file!");
  ENGINE_ASSERT(f.peek() not_eq std::ifstream::traits_type::eof(),
                "scene file is empty!");
  json data = json::parse(f);
  f.close();

  read(data);
}

void Scene::read(const json& data) {
  if (data.contains("assets")) {
    if (data.at("assets").contains("shaders")) {
      for (const auto& [key, value] : data.at("assets").at("shaders").items()) {
        m_shaders[key] = value;
      }
    }
    if (data.at("assets").contains("textures")) {
      for (const auto& [key, value] :
           data.at("assets").at("textures").items()) {
        m_textures[key] = value;
      }
    }
    if (data.at("assets").contains("models")) {
      for (const auto& [key, value] : data.at("assets").at("models").items()) {
        m_models[key] = value;
      }
    }
    if (data.at("assets").contains("prefabs")) {
      for (const auto& [key, value] : data.at("assets").at("prefabs").items()) {
        m_prefabs[key] = value;
      }
    }
  }
}

const std::map<std::string, std::string, NumericComparator>& Scene::to_map() {
  if (not m_info.empty()) {
    return m_info;
  }

  m_info["type"] = "scene";
  m_info["path"] = m_path;
  m_info["shaders"] = std::to_string(m_shaders.size());
  m_info["textures"] = std::to_string(m_textures.size());
  m_info["models"] = std::to_string(m_models.size());
  m_info["prefabs"] = std::to_string(m_prefabs.size());

  return m_info;
}

bool Scene::operator==(const Asset& other) const {
  if (typeid(*this) not_eq typeid(other)) {
    ENGINE_ASSERT(false, "cannot compare scene with other asset type!");
  }
  const Scene& otherScene = static_cast<const Scene&>(other);
  return m_path == otherScene.m_path;
}
}