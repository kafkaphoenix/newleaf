#include "assets/scene.h"

#include "utils/timer.h"

namespace potatoengine::assets {
Scene::Scene(std::filesystem::path&& fp) : m_path(std::move(fp.string())) {
  std::ifstream f(fp);
  ENGINE_ASSERT(f.is_open(), "Failed to open scene file!");
  ENGINE_ASSERT(f.peek() not_eq std::ifstream::traits_type::eof(),
                "Scene file is empty!");
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

  m_info["Type"] = "Scene";
  m_info["Path"] = m_path;
  m_info["Shaders"] = std::to_string(m_shaders.size());
  m_info["Textures"] = std::to_string(m_textures.size());
  m_info["Models"] = std::to_string(m_models.size());
  m_info["Prefabs"] = std::to_string(m_prefabs.size());

  return m_info;
}

bool Scene::operator==(const Asset& other) const {
  if (typeid(*this) not_eq typeid(other)) {
    ENGINE_ASSERT(false, "Cannot compare scene with other asset type!");
  }
  const Scene& otherScene = static_cast<const Scene&>(other);
  return m_path == otherScene.m_path;
}
}