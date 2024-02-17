#include "scene/sceneFactory.h"

#include <nlohmann/json.hpp>

#include "assets/model.h"
#include "assets/prefab.h"
#include "assets/scene.h"
#include "assets/shader.h"
#include "assets/texture.h"
#include "scene/components/core/cDeleted.h"
#include "scene/components/core/cName.h"
#include "scene/components/core/cTag.h"
#include "scene/components/core/cUUID.h"
#include "utils/timer.h"
#include "utils/uuid.h"

using json = nlohmann::json;
using namespace entt::literals;

namespace potatoengine {

SceneFactory::SceneFactory() : m_entityFactory() {}

entt::entity SceneFactory::createEntity(std::string_view prefab_id,
                                        std::string&& prototypeID,
                                        entt::registry& registry,
                                        std::string&& name,
                                        std::optional<std::string> tag,
                                        std::optional<uint32_t> uuid) {
  UUID _uuid = uuid.has_value() ? UUID(uuid.value()) : UUID();
  std::string _tag = tag.has_value() ? tag.value() : prototypeID;
  entt::entity e = cloneEntity(
    m_entityFactory.getPrototypes(prefab_id, {prototypeID}).at(prototypeID),
    _uuid, registry, std::move(name), std::move(_tag));
  return e;
}

entt::entity SceneFactory::cloneEntity(const entt::entity& e, uint32_t uuid,
                                       entt::registry& registry,
                                       std::optional<std::string> name,
                                       std::optional<std::string> tag) {
  entt::entity cloned = registry.create();

  for (const auto& curr : registry.storage()) {
    if (auto& storage = curr.second; storage.contains(e)) {
      storage.push(cloned, storage.value(e));
      entt::meta_type cType = entt::resolve(storage.type());
      entt::meta_any cData = cType.construct(storage.value(e));
      entt::meta_func triggerEventFunc = cType.func("onComponentCloned"_hs);
      if (triggerEventFunc) {
        triggerEventFunc.invoke({}, e, cData);
      }
    }
  }

  registry.emplace<CUUID>(cloned, uuid);
  if (name.has_value()) {
    registry.emplace<CName>(cloned, std::move(name.value()));
  }
  if (tag.has_value()) {
    registry.emplace<CTag>(cloned, std::move(tag.value()));
  }
  m_dirtyMetrics = true;
  m_dirtyNamedEntities = true;
  return cloned;
}

void SceneFactory::createScene(
  std::string scene_id, std::string scene_path,
  const std::unique_ptr<assets::AssetsManager>& assets_manager,
  const std::unique_ptr<RenderManager>& render_manager,
  entt::registry& registry) {
  Timer timer;
  ENGINE_INFO("Creating scene...");

  assets::Scene scene = assets::Scene(scene_path);
  createShaderPrograms(scene, assets_manager, render_manager);
  ENGINE_INFO("Shader programs creation TIME: {:.6f}s", timer.getSeconds());
  timer.reset();
  createTextures(scene, assets_manager);
  ENGINE_INFO("Textures creation TIME: {:.6f}s", timer.getSeconds());
  timer.reset();
  createModels(scene, assets_manager);
  ENGINE_INFO("Models creation TIME: {:.6f}s", timer.getSeconds());
  timer.reset();
  createEntitiesFromPrefabs(scene, assets_manager, render_manager, registry);
  ENGINE_INFO("Entities creation TIME: {:.6f}s", timer.getSeconds());
  timer.reset();
  createChildrenScenes(scene, assets_manager);
  ENGINE_INFO("Children scenes creation TIME: {:.6f}s", timer.getSeconds());
  timer.reset();
  assets_manager->load<assets::Scene>(scene_id, std::move(scene));
  ENGINE_INFO("Scene {} creation TIME: {:.6f}s", scene_id, timer.getSeconds());
  m_active_scene = std::move(scene_id);

  m_dirtyMetrics = true;
  m_dirtyNamedEntities = true;
}

void SceneFactory::reloadScene(
  const std::unique_ptr<assets::AssetsManager>& assets_manager,
  const std::unique_ptr<RenderManager>& render_manager,
  entt::registry& registry, bool reload_prototypes) {
  Timer timer;
  ENGINE_ASSERT(not m_active_scene.empty(), "No scene is active!");
  ENGINE_INFO("Reloading scene {}", m_active_scene);

  const auto& scene = assets_manager->get<assets::Scene>(m_active_scene);
  if (reload_prototypes) {
    registry.clear(); // delete instances and prototypes but reusing them later
    m_entityFactory.clearPrototypes();

    for (const auto& [prefab_name, options] : scene->getPrefabs()) {
      ENGINE_TRACE("Reloading scene prefabs prototypes...");
      const auto& prefab = assets_manager->get<assets::Prefab>(prefab_name);
      m_entityFactory.createPrototypes(
        prefab_name, prefab->getTargetedPrototypes(), registry, assets_manager);
    }
  } else {
    auto to_destroy = registry.view<CUUID>();
    registry.destroy(to_destroy.begin(), to_destroy.end());
  }

  ENGINE_INFO("Scene {} reloading TIME: {:.6f}s", m_active_scene,
              timer.getSeconds());

  m_dirtyMetrics = true;
  m_dirtyNamedEntities = true;
}

void SceneFactory::clearScene(
  const std::unique_ptr<RenderManager>& render_manager,
  entt::registry& registry) {
  ENGINE_ASSERT(not m_active_scene.empty(), "No scene is active!");
  ENGINE_WARN("Clearing scene {}", m_active_scene);

  registry.clear(); // soft delete / = {};  would delete them completely but
                    // does not invoke signals/mixin methods
  m_entityFactory.clearPrototypes();
  render_manager->clear();
  m_active_scene.clear();
  m_metrics.clear();
  m_namedEntities.clear();
  m_dirtyMetrics = false;
  m_dirtyNamedEntities = false;
}

void SceneFactory::createShaderPrograms(
  const assets::Scene& scene,
  const std::unique_ptr<assets::AssetsManager>& assets_manager,
  const std::unique_ptr<RenderManager>& render_manager) {
  for (const auto& [shader_program, shader_program_data] : scene.getShaders()) {
    for (const auto& [shader_type, filepath] : shader_program_data.items()) {
      assets_manager->load<assets::Shader>(shader_type, filepath);
    } // TODO maybe remove shader as asset?
    render_manager->addShaderProgram(std::string(shader_program),
                                     assets_manager);
  }
}

void SceneFactory::createTextures(
  const assets::Scene& scene,
  const std::unique_ptr<assets::AssetsManager>& assets_manager) {
  for (const auto& [texture, options] : scene.getTextures()) {
    bool flipY = options.contains("flip_vertically")
                   ? options.at("flip_vertically").get<bool>()
                   : true;
    bool flipOption = flipY ? assets::Texture::FLIP_VERTICALLY
                            : assets::Texture::DONT_FLIP_VERTICALLY;
    assets_manager->load<assets::Texture>(
      texture, options.at("filepath").get<std::string>(),
      options.at("type").get<std::string>(), flipOption);
  }
}

void SceneFactory::createModels(
  const assets::Scene& scene,
  const std::unique_ptr<assets::AssetsManager>& assets_manager) {
  for (const auto& [model, filepath] : scene.getModels()) {
    assets_manager->load<assets::Model>(model, filepath);
  }
}

void SceneFactory::createEntitiesFromPrefabs(
  const assets::Scene& scene,
  const std::unique_ptr<assets::AssetsManager>& assets_manager,
  const std::unique_ptr<RenderManager>& render_manager,
  entt::registry& registry) {
  for (const auto& [prefab_name, options] : scene.getPrefabs()) {
    auto prefab = assets::Prefab(
      options.at("filepath").get<std::string>(),
      options.at("targeted_prototypes").get<std::vector<std::string>>());
    ENGINE_TRACE("Creating scene prototypes for prefab {}...", prefab_name);
    std::vector<std::string> targetedPrototypes =
      prefab.getTargetedPrototypes();
    assets_manager->load<assets::Prefab>(prefab_name, std::move(prefab));
    m_entityFactory.createPrototypes(prefab_name, targetedPrototypes, registry,
                                     assets_manager);
  }
}

void SceneFactory::createChildrenScenes(
  const assets::Scene& scene,
  const std::unique_ptr<assets::AssetsManager>& assets_manager) {
  // TODO: implement
}

void SceneFactory::removeEntity(entt::entity& e, entt::registry& registry) {
  registry.emplace<CDeleted>(e);
  m_dirtyMetrics = true;
  m_dirtyNamedEntities = true;
}

const std::map<std::string, std::string, NumericComparator>&
SceneFactory::getMetrics(entt::registry& registry) {
  if (not m_dirtyMetrics) {
    return m_metrics;
  }

  m_metrics.clear();
  m_metrics["Active scene"] = m_active_scene;
  int total = registry.storage<entt::entity>().in_use();
  int created = registry.storage<entt::entity>().size();
  int prototypes = 0;
  for (const auto& [key, prototypesMap] : m_entityFactory.getAllPrototypes()) {
    prototypes += prototypesMap.size();
    m_metrics["Prefab " + key + " Prototypes Alive"] =
      std::to_string(prototypesMap.size());
  }
  m_metrics["Prototypes Total Alive"] = std::to_string(prototypes);
  m_metrics["Instances Total Alive"] = std::to_string(total - prototypes);
  m_metrics["Entities Total Alive"] = std::to_string(total);
  m_metrics["Entities Total Created"] = std::to_string(created);
  m_metrics["Entities Total Released"] = std::to_string(created - total);
  m_dirtyMetrics = false;

  return m_metrics;
}

const std::map<std::string, entt::entity, NumericComparator>&
SceneFactory::getNamedEntities(entt::registry& registry) {
  if (not m_dirtyNamedEntities) {
    return m_namedEntities;
  }

  m_namedEntities.clear();
  registry.view<CName, CUUID>().each(
    [&](entt::entity e, const CName& cName, const CUUID& cUUID) {
      m_namedEntities.emplace(cName.name, e);
    });
  m_dirtyNamedEntities = false;

  return m_namedEntities;
}

}