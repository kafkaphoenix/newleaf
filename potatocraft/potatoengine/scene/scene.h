#pragma once

#include <entt/entt.hpp>

#include "potatoengine/assets/assetsManager.h"
#include "potatoengine/core/time.h"
#include "potatoengine/renderer/renderer.h"
#include "potatoengine/scene/entityFactory.h"
#include "potatoengine/utils/uuid.h"

namespace potatoengine {
class Entity;

class Scene {
   public:
    Scene(std::weak_ptr<AssetsManager> am);
    Entity create(assets::PrefabID id, const std::optional<uint32_t>& uuid = std::nullopt);
    Entity clone(Entity e, uint32_t uuid);
    void clear(Entity& e);
    void remove(Entity& e);
    void print();
    void onUpdate(Time ts, std::weak_ptr<Renderer> r);
    Entity getEntity(std::string_view name);
    Entity getEntity(UUID& uuid);
    void createPrefab(assets::PrefabID id);
    void createPrefabs();
    void updatePrefab(assets::PrefabID id);
    void updatePrefabs();

    template <typename T>
    void onComponentAdded(Entity e, T& c);

    template <typename T>
    void onComponentCloned(Entity e, T& c);

   private:
    EntityFactory m_entityFactory;
    std::weak_ptr<AssetsManager> m_assetsManager;
    entt::registry m_registry;

    friend class Entity;
};
}