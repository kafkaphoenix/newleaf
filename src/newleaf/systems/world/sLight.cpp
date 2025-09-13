#include "sLight.h"

#include "../../application/application.h"
#include "../../components/meta/cUUID.h"
#include "../../components/physics/cTransform.h"
#include "../../components/world/cLight.h"

using namespace entt::literals;

namespace nl {

void LightSystem::update(entt::registry& registry, const Time& ts) {
  if (Application::get().is_paused()) {
    return;
  }

  // TODO this only works with one light fix
  registry.view<CLight, CTransform, CUUID>().each(
    [&](const CLight& cLight, const CTransform& cTransform, const CUUID& cUUID) {
      if (not cLight.enabled) {
        entt::monostate<"light_enabled"_hs>{} = false;
        return;
      }
      entt::monostate<"light_enabled"_hs>{} = true;
      entt::monostate<"light_position"_hs>{} = cTransform.position;
      entt::monostate<"light_color"_hs>{} = cLight.color;
      entt::monostate<"light_intensity"_hs>{} = cLight.intensity;
      entt::monostate<"light_range"_hs>{} = cLight.range;
      entt::monostate<"light_inner_cone_angle"_hs>{} = cLight.inner_cone_angle;
      entt::monostate<"light_outer_cone_angle"_hs>{} = cLight.outer_cone_angle;
    });

    // TODO update light position based on light type (directional, cone, area, ...)
    // or time etc
}
}