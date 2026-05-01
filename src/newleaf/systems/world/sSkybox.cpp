#include "sSkybox.h"

#include <glm/glm.hpp>

#include "../../application/application.h"
#include "../../components/meta/cUUID.h"
#include "../../components/physics/cRigidBody.h"
#include "../../components/physics/cTransform.h"
#include "../../components/render/cBlendTexture.h"
#include "../../components/render/cTexture.h"
#include "../../components/world/cSkybox.h"
#include "../../components/world/cTime.h"

using namespace entt::literals;
namespace nl {

void SkyboxSystem::update(entt::registry& registry, const Time& ts) {
  if (Application::get().is_paused()) {
    return;
  }

  entt::entity clock = registry.view<CTime, CUUID>().front();
  APP_ASSERT(clock not_eq entt::null, "no active clock found!");
  const CTime& cTime = registry.get<CTime>(clock);

  registry.view<CSkybox, CTexture, CTransform, CRigidBody, CUUID>().each(
    [&](entt::entity e, const CSkybox& cSkybox, const CTexture& cTexture, CTransform& cTransform,
        const CRigidBody& cRigidBody, const CUUID& cUUID) {
      CBlendTexture* cSkyboxBlend = registry.try_get<CBlendTexture>(e);
      if (cSkyboxBlend) {
        float blend_factor = 0.f;
        if (cTime.current_hour >= cTime.night_start and cTime.current_hour < cTime.day_transition_start) {
          // night
          blend_factor = 0.f;
        } else if (cTime.current_hour >= cTime.day_transition_start and cTime.current_hour < cTime.day_start) {
          // morning
          blend_factor = (cTime.current_minute + (cTime.current_hour - cTime.day_transition_start) * cTime.fps) / 120.f;
        } else if (cTime.current_hour >= cTime.day_start and cTime.current_hour < cTime.night_transition_start) {
          // day
          blend_factor = 1.f;
        } else if (cTime.current_hour >= cTime.night_transition_start and cTime.current_hour < cTime.night_start) {
          // late afternoon
          blend_factor =
            1.f - ((cTime.current_minute + (cTime.current_hour - cTime.night_transition_start) * cTime.fps) / 120.f);
        }
        cSkyboxBlend->blend_factor = blend_factor;
      }

      // this can't be in init because clock doesn't exist yet when register system triggers init
      if (cRigidBody.kinematic) {
        float rotation = 0.f;
        if (cSkybox.rotation_speed > 0.f) {
          rotation = cSkybox.rotation_speed;
        } else {
          // to avoid rotating fps frames per second instead of one second we need to divide by fps
          float rotationAnglePerSecond = (360.f / (cTime.day_length * 3600.f)) / cTime.fps;
          rotation = rotationAnglePerSecond * cTime.acceleration;
        }
        cTransform.rotation = glm::angleAxis(glm::radians(rotation), glm::vec3(0.f, 1.f, 0.f)) * cTransform.rotation;
      }
    });
}
}