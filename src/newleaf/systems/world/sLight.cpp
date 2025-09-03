#include "sLight.h"

#include "../../application/application.h"
#include "../../components/meta/cUUID.h"
#include "../../components/physics/cTransform.h"
#include "../../components/world/cLight.h"

using namespace entt::literals;

namespace nl {

void LightSystem::init(entt::registry& registry) {
  registry.view<CLight, CTransform, CUUID>().each(
    [&](const CLight& cLight, const CTransform& cTransform, const CUUID& cUUID) {
      entt::monostate<"light_position"_hs>{} = cTransform.position;
      entt::monostate<"light_color"_hs>{} = cLight.color;
    });
}

void LightSystem::update(entt::registry& registry, const Time& ts) {
  if (Application::get().is_paused()) {
    return;
  }

  // TODO this only works with one light fix
  registry.view<CLight, CTransform, CUUID>().each(
    [&](const CLight& cLight, const CTransform& cTransform, const CUUID& cUUID) {
      entt::monostate<"light_position"_hs>{} = cTransform.position;
      entt::monostate<"light_color"_hs>{} = cLight.color;
    });
}
}