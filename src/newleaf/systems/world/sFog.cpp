#include "sFog.h"

#include <glm/glm.hpp>

#include "../../application/application.h"
#include "../../components/meta/cUUID.h"
#include "../../components/world/cFog.h"

using namespace entt::literals;

namespace nl {

void FogSystem::init(entt::registry& registry) {
  registry.view<CFog, CUUID>().each([&](const CFog& cFog, const CUUID& cUUID) {
    entt::monostate<"fog_density"_hs>{} = cFog.density;
    entt::monostate<"fog_gradient"_hs>{} = cFog.gradient;
    entt::monostate<"fog_color"_hs>{} = cFog.color;
  });
}

void FogSystem::update(entt::registry& registry, const Time& ts) {
  if (Application::get().is_paused()) {
    return;
  }

  registry.view<CFog, CUUID>().each([&](const CFog& cFog, const CUUID& cUUID) {
    entt::monostate<"fog_density"_hs>{} = cFog.density;
    entt::monostate<"fog_gradient"_hs>{} = cFog.gradient;
    entt::monostate<"fog_color"_hs>{} = cFog.color;
  });
}
}