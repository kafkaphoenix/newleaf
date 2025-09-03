#include "sSkybox.h"

#include <glm/glm.hpp>

#include "../../application/application.h"
#include "../../components/graphics/cTexture.h"
#include "../../components/meta/cUUID.h"
#include "../../components/physics/cRigidBody.h"
#include "../../components/physics/cTransform.h"
#include "../../components/world/cSkybox.h"
#include "../../components/world/cTime.h"

using namespace entt::literals;

namespace nl {

SkyboxSystem::~SkyboxSystem() { entt::monostate<"use_sky_blending"_hs>{} = 0.f; }

void SkyboxSystem::init(entt::registry& registry) {
  registry.view<CSkybox, CTransform, CRigidBody, CTime, CTexture, CUUID>().each(
    [&](const CSkybox& cSkybox, CTransform& cTransform, const CRigidBody& cRigidBody, const CTime& cTime,
        CTexture& cTexture, const CUUID& cUUID) {
      if (cTexture.draw_mode == CTexture::DrawMode::texture_blend) {
        float blend_factor = 0.f;
        if (cTime.current_hour >= cTime.night_start and cTime.current_hour < cTime.day_transition_start) {
          blend_factor = 0.f;
        } else if (cTime.current_hour >= cTime.day_transition_start and cTime.current_hour < cTime.day_start) {
          blend_factor = (cTime.current_minute + (cTime.current_hour - cTime.day_transition_start) * cTime.fps) / 120.f;
        } else if (cTime.current_hour >= cTime.day_start and cTime.current_hour < cTime.night_transition_start) {
          blend_factor = 1.f;
        } else if (cTime.current_hour >= cTime.night_transition_start and cTime.current_hour < cTime.night_start) {
          blend_factor =
            1.f - ((cTime.current_minute + (cTime.current_hour - cTime.night_transition_start) * cTime.fps) / 120.f);
        }
        cTexture.blend_factor = blend_factor;
        entt::monostate<"sky_blend_factor"_hs>{} = blend_factor;
        entt::monostate<"use_sky_blending"_hs>{} = 1.f;
      } else {
        entt::monostate<"use_sky_blending"_hs>{} = 0.f;
      }
    });
}

void SkyboxSystem::update(entt::registry& registry, const Time& ts) {
  if (Application::get().is_paused()) {
    return;
  }

  registry.view<CSkybox, CTransform, CRigidBody, CTime, CTexture, CUUID>().each(
    [&](const CSkybox& cSkybox, CTransform& cTransform, const CRigidBody& cRigidBody, const CTime& cTime,
        CTexture& cTexture, const CUUID& cUUID) {
      if (cTexture.draw_mode == CTexture::DrawMode::texture_blend) {
        float blend_factor = 0.f;
        if (cTime.current_hour >= cTime.night_start and cTime.current_hour < cTime.day_transition_start) {
          blend_factor = 0.f;
        } else if (cTime.current_hour >= cTime.day_transition_start and cTime.current_hour < cTime.day_start) {
          blend_factor = (cTime.current_minute + (cTime.current_hour - cTime.day_transition_start) * cTime.fps) / 120.f;
        } else if (cTime.current_hour >= cTime.day_start and cTime.current_hour < cTime.night_transition_start) {
          blend_factor = 1.f;
        } else if (cTime.current_hour >= cTime.night_transition_start and cTime.current_hour < cTime.night_start) {
          blend_factor =
            1.f - ((cTime.current_minute + (cTime.current_hour - cTime.night_transition_start) * cTime.fps) / 120.f);
        }
        cTexture.blend_factor = blend_factor;
        entt::monostate<"sky_blend_factor"_hs>{} = blend_factor;
        entt::monostate<"use_sky_blending"_hs>{} = 1.f;
      } else {
        entt::monostate<"use_sky_blending"_hs>{} = 0.f;
      }

      if (cRigidBody.kinematic) {
        // TODO cActiveSkybox so we can set different ones in different planets
        float rotation = 0.f;
        if (cSkybox.rotation_speed > 0.f) {
          rotation = cSkybox.rotation_speed;
        } else {
          // to avoid rotating fps frames per second instead of one we need to
          // divide by fps
          float rotationAnglePerSecond = (360.f / (cTime.day_length * 3600.f)) / cTime.fps;
          rotation = rotationAnglePerSecond * cTime.acceleration;
        }
        cTransform.rotation = glm::angleAxis(glm::radians(rotation), glm::vec3(0.f, 1.f, 0.f)) * cTransform.rotation;
      }
    });
}
}