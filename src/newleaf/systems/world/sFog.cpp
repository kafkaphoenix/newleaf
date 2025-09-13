#include "sFog.h"

#include <glm/glm.hpp>

#include "../../application/application.h"
#include "../../components/meta/cUUID.h"
#include "../../components/world/cFog.h"
#include "../../components/world/cTime.h"
#include "../../logging/log_manager.h"

using namespace entt::literals;

namespace nl {

FogSystem::~FogSystem() { entt::monostate<"fog_enabled"_hs>{} = 0.f; }

void FogSystem::update(entt::registry& registry, const Time& ts) {
  if (Application::get().is_paused()) {
    return;
  }

  entt::entity clock = registry.view<CTime, CUUID>().front();
  APP_ASSERT(clock not_eq entt::null, "no active clock found!");
  const CTime& cTime = registry.get<CTime>(clock);

  registry.view<CFog, CUUID>().each([&](CFog& cFog, const CUUID& cUUID) {
    if (not cFog.enabled) {
      entt::monostate<"fog_enabled"_hs>{} = false;
      return;
    }
    entt::monostate<"fog_enabled"_hs>{} = true;
    entt::monostate<"fog_lower_limit"_hs>{} = cFog.lower_limit;
    entt::monostate<"fog_upper_limit"_hs>{} = cFog.upper_limit;

    glm::vec4 color = cFog.color;
    float density = cFog.density;
    float gradient = cFog.gradient;
    float blend_factor = (cTime.current_minute + (cTime.current_hour - cTime.day_transition_start) * cTime.fps) / 120.f;
    if (cTime.current_hour >= cTime.night_start and cTime.current_hour < cTime.day_transition_start) {
      // night: medium bluish fog
      color = glm::vec4(0.25f, 0.28f, 0.31f, 1.0f);
      density = 0.5f;
      gradient = 1.0f;
    } else if (cTime.current_hour >= cTime.day_transition_start and cTime.current_hour < cTime.day_start) {
      // morning: transition night → warm sunrise
      color = glm::mix(glm::vec4(0.25f, 0.28f, 0.35f, 1.0f), glm::vec4(0.85f, 0.65f, 0.45f, 1.0f), blend_factor);
      density = glm::mix(0.5f, 0.3f, blend_factor);
      gradient = glm::mix(1.0f, 0.75f, blend_factor);
    } else if (cTime.current_hour >= cTime.day_start and cTime.current_hour < cTime.night_transition_start) {
      // day: transition warm sunrise → clear sky
      color = glm::mix(glm::vec4(0.85f, 0.65f, 0.45f, 1.0f), glm::vec4(0.8f, 0.9f, 1.0f, 1.0f), blend_factor);
      density = glm::mix(0.3f, 0.1f, blend_factor);
      gradient = glm::mix(0.75f, 0.5f, blend_factor);
    } else if (cTime.current_hour >= cTime.night_transition_start and cTime.current_hour < cTime.night_start) {
      // afternoon: transition clear sky → bluish fog
      color = glm::mix(glm::vec4(0.8f, 0.9f, 1.0f, 1.0f), glm::vec4(0.25f, 0.28f, 0.31f, 1.0f), blend_factor);
      density = glm::mix(0.1f, 0.5f, blend_factor);
      gradient = glm::mix(0.5f, 1.0f, blend_factor);
    }
    cFog.color = color;
    cFog.density = density;
    cFog.gradient = gradient;
    entt::monostate<"fog_color"_hs>{} = color;
    entt::monostate<"fog_density"_hs>{} = density;
    entt::monostate<"fog_gradient"_hs>{} = gradient;
  });
}
}