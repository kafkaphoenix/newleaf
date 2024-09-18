#include "scene_factory.h"

#include <nlohmann/json.hpp>

#include "../assets/model.h"
#include "../assets/prefab.h"
#include "../assets/scene.h"
#include "../assets/shader.h"
#include "../assets/texture.h"
#include "../components/meta/cDeleted.h"
#include "../components/meta/cName.h"
#include "../components/meta/cTag.h"
#include "../components/meta/cUUID.h"
#include "../components/register.h"
#include "../logging/log_manager.h"
#include "../utils/assert.h"
#include "../utils/timer.h"
#include "../utils/uuid.h"

using json = nlohmann::json;
using namespace entt::literals;

namespace nl {

SceneFactory::SceneFactory() : m_entity_factory() {
  ENGINE_TRACE("registering engine components...");
  register_components();
}

entt::entity SceneFactory::create_entity(std::string_view prefab_id,
                                         std::string&& prototype_id,
                                         entt::registry& registry,
                                         std::string&& name,
                                         std::optional<std::string> tag,
                                         std::optional<uint32_t> uuid) {
  UUID _uuid = uuid.has_value() ? UUID(uuid.value()) : UUID();
  std::string _tag = tag.has_value() ? tag.value() : prototype_id;
  entt::entity e = clone_entity(
    m_entity_factory.get_prototypes(prefab_id, {prototype_id}).at(prototype_id),
    _uuid, registry, std::move(name), std::move(_tag));
  return e;
}

entt::entity SceneFactory::clone_entity(const entt::entity& e, uint32_t uuid,
                                        entt::registry& registry,
                                        std::optional<std::string> name,
                                        std::optional<std::string> tag) {
  entt::entity cloned = registry.create();

  for (const auto& curr : registry.storage()) {
    if (auto& storage = curr.second; storage.contains(e)) {
      storage.push(cloned, storage.value(e));
      entt::meta_type cType = entt::resolve(storage.type());
      entt::meta_any cData = cType.construct(storage.value(e));
      entt::meta_func triggerEventFunc = cType.func("on_component_cloned"_hs);
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
  m_dirty_metrics = true;
  m_dirty_named_entities = true;
  return cloned;
}

void SceneFactory::create_scene(
  std::string scene_id, std::string scene_path,
  const std::unique_ptr<AssetsManager>& assets_manager,
  const std::unique_ptr<RenderManager>& render_manager,
  entt::registry& registry) {
  Timer timer;
  ENGINE_INFO("creating scene...");

  Scene scene = Scene(scene_path);
  create_shader_programs(scene, assets_manager, render_manager);
  ENGINE_INFO("shader programs creation time: {:.6f}s", timer.get_seconds());
  timer.reset();
  create_textures(scene, assets_manager);
  ENGINE_INFO("textures creation time: {:.6f}s", timer.get_seconds());
  timer.reset();
  create_models(scene, assets_manager);
  ENGINE_INFO("models creation time: {:.6f}s", timer.get_seconds());
  timer.reset();
  create_prototypes(scene, assets_manager, render_manager, registry);
  ENGINE_INFO("entities creation time: {:.6f}s", timer.get_seconds());
  timer.reset();
  assets_manager->load<Scene>(scene_id, std::move(scene));
  ENGINE_INFO("scene {} creation time: {:.6f}s", scene_id, timer.get_seconds());
  m_active_scene = std::move(scene_id);

  m_dirty_metrics = true;
  m_dirty_named_entities = true;
}

void SceneFactory::reload_scene(
  const std::unique_ptr<AssetsManager>& assets_manager,
  const std::unique_ptr<RenderManager>& render_manager,
  entt::registry& registry, bool reload_prototypes) {
  Timer timer;
  ENGINE_ASSERT(not m_active_scene.empty(), "no scene is active!");
  ENGINE_INFO("reloading scene {}", m_active_scene);

  const auto& scene = assets_manager->get<Scene>(m_active_scene);
  if (reload_prototypes) {
    registry.clear(); // delete instances and prototypes but reusing them later
    m_entity_factory.clear_prototypes();

    for (const auto& [prefab_name, options] : scene->get_prefabs()) {
      ENGINE_TRACE("reloading scene prototypes...");
      const auto& prefab = assets_manager->get<Prefab>(prefab_name);
      m_entity_factory.create_prototypes(
        prefab_name, prefab->get_target_prototypes(), registry, assets_manager);
    }
  } else {
    auto to_destroy = registry.view<CUUID>();
    registry.destroy(to_destroy.begin(), to_destroy.end());
  }

  ENGINE_INFO("scene {} reloading time: {:.6f}s", m_active_scene,
              timer.get_seconds());

  m_dirty_metrics = true;
  m_dirty_named_entities = true;
}

void SceneFactory::clear_scene(
  const std::unique_ptr<RenderManager>& render_manager,
  entt::registry& registry) {
  ENGINE_ASSERT(not m_active_scene.empty(), "no scene is active!");
  ENGINE_WARN("clearing scene {}", m_active_scene);

  registry.clear(); // soft delete / = {};  would delete them completely but
                    // does not invoke signals/mixin methods
  m_entity_factory.clear_prototypes();
  render_manager->clear();
  m_active_scene.clear();
  m_metrics.clear();
  m_named_entities.clear();
  m_dirty_metrics = false;
  m_dirty_named_entities = false;
}

void SceneFactory::create_shader_programs(
  const Scene& scene, const std::unique_ptr<AssetsManager>& assets_manager,
  const std::unique_ptr<RenderManager>& render_manager) {
  for (const auto& [shader_program, shader_program_data] :
       scene.get_shader_programs()) {
    for (const auto& [shader_type, path] : shader_program_data.items()) {
      assets_manager->load<Shader>(shader_type, path);
    } // TODO maybe delete shader as asset?
    render_manager->add_shader_program(std::string(shader_program),
                                       assets_manager);
  }
}

void SceneFactory::create_textures(
  const Scene& scene, const std::unique_ptr<AssetsManager>& assets_manager) {
  for (const auto& [texture, options] : scene.get_textures()) {
    bool flip_y = options.contains("flip_vertically")
                    ? options.at("flip_vertically").get<bool>()
                    : true;
    bool flip_option =
      flip_y ? Texture::FLIP_VERTICALLY : Texture::DONT_FLIP_VERTICALLY;
    assets_manager->load<Texture>(
      texture, options.at("path").get<std::string>(),
      options.at("type").get<std::string>(), flip_option);
  }
}

void SceneFactory::create_models(
  const Scene& scene, const std::unique_ptr<AssetsManager>& assets_manager) {
  for (const auto& [model, path] : scene.get_models()) {
    assets_manager->load<Model>(model, path);
  }
}

void SceneFactory::create_prototypes(
  const Scene& scene, const std::unique_ptr<AssetsManager>& assets_manager,
  const std::unique_ptr<RenderManager>& render_manager,
  entt::registry& registry) {
  for (const auto& [prefab_name, options] : scene.get_prefabs()) {
    auto prefab =
      Prefab(options.at("path").get<std::string>(),
             options.at("target_prototypes").get<std::vector<std::string>>());
    ENGINE_TRACE("creating prototypes from prefab {}...", prefab_name);
    std::vector<std::string> targetPrototypes = prefab.get_target_prototypes();
    assets_manager->load<Prefab>(prefab_name, std::move(prefab));
    m_entity_factory.create_prototypes(prefab_name, targetPrototypes, registry,
                                       assets_manager);
  }
}

void SceneFactory::delete_entity(entt::entity& e, entt::registry& registry) {
  registry.emplace<CDeleted>(e);
  m_dirty_metrics = true;
  m_dirty_named_entities = true;
}

const std::map<std::string, std::string, NumericComparator>&
SceneFactory::get_metrics(entt::registry& registry) {
  if (not m_dirty_metrics) {
    return m_metrics;
  }

  m_metrics.clear();
  m_metrics["active_scene"] = m_active_scene;
  int total = registry.storage<entt::entity>().in_use();
  int created = registry.storage<entt::entity>().size();
  int prototypes = 0;
  for (const auto& [key, prototypesMap] :
       m_entity_factory.get_all_prototypes()) {
    prototypes += prototypesMap.size();
    m_metrics["prototypes_alive_from_prefab_" + key] =
      std::to_string(prototypesMap.size());
  }
  m_metrics["prototypes_total_alive"] = std::to_string(prototypes);
  m_metrics["instances_total_alive"] = std::to_string(total - prototypes);
  m_metrics["entities_total_alive"] = std::to_string(total);
  m_metrics["entities_total_created"] = std::to_string(created);
  m_metrics["entities_total_released"] = std::to_string(created - total);
  m_dirty_metrics = false;

  return m_metrics;
}

const std::map<std::string, entt::entity, NumericComparator>&
SceneFactory::get_named_entities(entt::registry& registry) {
  if (not m_dirty_named_entities) {
    return m_named_entities;
  }

  m_named_entities.clear();
  registry.view<CName, CUUID>().each(
    [&](entt::entity e, const CName& cName, const CUUID& cUUID) {
      m_named_entities.emplace(cName.name, e);
    });
  m_dirty_named_entities = false;

  return m_named_entities;
}

}