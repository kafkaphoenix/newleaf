#include "sRender.h"

#include <memory>

#include <glm/glm.hpp>

#include "../../application/application.h"
#include "../../components/camera/cActiveCamera.h"
#include "../../components/camera/cCamera.h"
#include "../../components/camera/cDistanceFromCamera.h"
#include "../../components/graphics/cBlendColor.h"
#include "../../components/graphics/cBlendTexture.h"
#include "../../components/graphics/cBody.h"
#include "../../components/graphics/cColor.h"
#include "../../components/graphics/cFBO.h"
#include "../../components/graphics/cMaterial.h"
#include "../../components/graphics/cMesh.h"
#include "../../components/graphics/cReflection.h"
#include "../../components/graphics/cShaderProgram.h"
#include "../../components/graphics/cShape.h"
#include "../../components/graphics/cTexture.h"
#include "../../components/graphics/cTextureAtlas.h"
#include "../../components/graphics/cTransparent.h"
#include "../../components/meta/cName.h"
#include "../../components/meta/cUUID.h"
#include "../../components/physics/cCollider.h"
#include "../../components/physics/cTransform.h"
#include "../../components/world/cSkybox.h"
#include "../../graphics/render_api.h"
#include "../../logging/log_manager.h"
#include "../../settings/settings_manager.h"

namespace nl {

void render(CTexture* cTexture, CBlendTexture* cBlendTexture, CTextureAtlas* cTextureAtlas, CColor* cColor,
            CBlendColor* cBlendColor, CMaterial* cMaterial, CReflection* cReflection, CSkybox* cSkybox,
            CTexture* cSkyboxTexture, CBlendTexture* cSkyboxBlend, CMesh* cMesh, const CTransform& cTransform,
            const CShaderProgram& cShaderProgram, CCollider* cCollider, CTransparent* cTransparent,
            RenderManager& render_manager) {
  if (cTransparent and cTransparent->transparent) {
    RenderAPI::toggle_culling(false);
  }
  if (cSkybox) {
    RenderAPI::set_depth_lequal();
  }
  // TODO refactor this to a system
  bool display_hitbox = false;
  if (cCollider) {
    display_hitbox = cCollider->display_hitbox or Application::get().get_settings_manager().display_collision_boxes;
  }
  ShaderProgram& sp = render_manager.get_shader_program(cShaderProgram.name);
  cMesh->bind_textures(sp, cTexture, cBlendTexture, cTextureAtlas,
                       cColor, cBlendColor, cMaterial, cReflection, cSkybox, cSkyboxTexture, cSkyboxBlend);
  render_manager.render(cMesh->get_vao(), cTransform.calculate(), cShaderProgram.name);
  cMesh->unbind_textures(cTexture, cTextureAtlas, cBlendTexture);
  if (cTransparent and cTransparent->transparent) {
    RenderAPI::toggle_culling(true);
  }
  if (cSkybox) {
    RenderAPI::set_depth_less();
  }
  if (cCollider and display_hitbox) {
    // TODO fix transparency so I can render this first
    // disabling culling is not working
    // this can't be in cmesh bind_textures right now
    sp.reset_active_uniforms();
    sp.use();
    sp.set_bool("display_hitbox", true);
    sp.set_vec4("hitbox_color", cCollider->color);
    sp.unuse();
    render_manager.render(cCollider->mesh.get_vao(), cTransform.calculate(), cShaderProgram.name);
  } else if (cCollider and not display_hitbox) {
    sp.use();
    sp.set_bool("display_hitbox", false);
    sp.unuse();
  }
}

void RenderSystem::update(entt::registry& registry, const Time& ts) {
  auto& app = Application::get();
  auto& render_manager = app.get_render_manager();

  // TODO: support more than one?
  entt::entity fbo = registry.view<CFBO, CUUID>().front();
  if (fbo not_eq entt::null) {
    const CFBO& cfbo = registry.get<CFBO>(fbo);
    const auto& default_FBO = render_manager.get_framebuffers().at(cfbo.fbo);
    default_FBO->bind_to_draw();
    RenderAPI::toggle_depth_test(true);
  }

  // FBOs are cleared in their own render pass at the end of the scene
  RenderAPI::clear();
  render_manager.reset_metrics();

  entt::entity camera = registry.view<CCamera, CActiveCamera, CTransform, CUUID>().front();
  APP_ASSERT(camera not_eq entt::null, "no active camera found!");
  CCamera& cCamera = registry.get<CCamera>(camera);
  const CTransform& cCameraTransform = registry.get<CTransform>(camera);
  cCamera.calculate_view(cCameraTransform.position, cCameraTransform.rotation);
  render_manager.begin_scene(cCamera.view, cCamera.projection, cCameraTransform.position);

  entt::entity sky = registry.view<CSkybox, CUUID>().front();
  CTexture* cSkyboxTexture;
  if (sky not_eq entt::null) {
    cSkyboxTexture = registry.try_get<CTexture>(sky);
    APP_ASSERT(cSkyboxTexture, "no skybox texture found!");
    CBlendTexture* cSkyboxBlend = registry.try_get<CBlendTexture>(sky);
  }

  if (render_manager.should_reorder()) {
    registry.sort<CDistanceFromCamera>(
      [](const CDistanceFromCamera& lhs, const CDistanceFromCamera& rhs) { return lhs.distance < rhs.distance; });
    registry.sort<CUUID, CDistanceFromCamera>();
  }

  registry.view<CTransform, CShaderProgram, CUUID>().each(
    [&](entt::entity e, const CTransform& cTransform, const CShaderProgram& cShaderProgram, const CUUID& cUUID) {
      // these two we will ignored for the entity sky
      CTexture* cTexture = registry.try_get<CTexture>(e);
      CBlendTexture* cBlendTexture = registry.try_get<CBlendTexture>(e);
      // not used by terrain or model
      CTextureAtlas* cTextureAtlas = registry.try_get<CTextureAtlas>(e);
      CColor* cColor = registry.try_get<CColor>(e);
      CBlendColor* cBlendColor = registry.try_get<CBlendColor>(e);
      CMaterial* cMaterial = registry.try_get<CMaterial>(e);
      CReflection* cReflection = registry.try_get<CReflection>(e);
      CTransparent* cTransparent = registry.try_get<CTransparent>(e);
      // TODO improve we check this component to see if the entity is a skybox
      // and disable depth test + avoid binding textures two times (sky and normal)
      // for the entity with CSkybox
      CSkybox* cSkybox = registry.try_get<CSkybox>(e);
      CBody* cBody = registry.try_get<CBody>(e);
      CMesh* cMesh = registry.try_get<CMesh>(e);
      CShape* cShape = registry.try_get<CShape>(e);
      CCollider* cCollider = registry.try_get<CCollider>(e);

      if (cShaderProgram.visible) {
        if (cMesh) { // TODO objects with one mesh unused
          if (not cTexture and not cTextureAtlas) {
            CName* cName = registry.try_get<CName>(e);
            if (cName) {
              APP_ASSERT(false, "no texture found for entity {} {}", cUUID.uuid, cName->name);
            } else {
              APP_ASSERT(false, "no texture found for entity {}", cUUID.uuid);
            }
          }

          render(cTexture, cBlendTexture, cTextureAtlas, cColor, cBlendColor, cMaterial, cReflection, cSkybox,
                 cSkyboxTexture, cBlendTexture, cMesh, cTransform, cShaderProgram, cCollider, cTransparent,
                 render_manager);
        } else if (cBody) { // models
          for (uint32_t i = 0; i < cBody->meshes.size(); ++i) {
            CMesh* mesh = cBody->meshes.at(i);
            CMaterial* material = cBody->materials.at(i);
            // TODO Add textures as CTexture
                 render(cTexture, cBlendTexture, cTextureAtlas, cColor, cBlendColor, material, cReflection, cSkybox,
                   cSkyboxTexture, cBlendTexture, mesh, cTransform, cShaderProgram, cCollider, cTransparent,
                   render_manager);
          }
        } else if (cShape) { // primitives
          if (not cTexture and not cTextureAtlas) {
            CName* cName = registry.try_get<CName>(e);
            if (cName) {
              APP_ASSERT(false, "no texture found for entity {} {}", cUUID.uuid, cName->name);
            } else {
              APP_ASSERT(false, "no texture found for entity {}", cUUID.uuid);
            }
          }

          for (auto& mesh : cShape->meshes) {
            render(cTexture, cBlendTexture, cTextureAtlas, cColor, cBlendColor, cMaterial, cReflection, cSkybox,
                   cSkyboxTexture, cBlendTexture, mesh.get(), cTransform, cShaderProgram, cCollider, cTransparent,
                   render_manager);
          }
        } else {
          CName* cName = registry.try_get<CName>(e);
          if (cName) {
            APP_ASSERT(false, "no mesh found for entity {} {}", cUUID.uuid, cName->name);
          } else {
            APP_ASSERT(false, "no mesh found for entity {}", cUUID.uuid);
          }
        }
      }
    });

  if (fbo not_eq entt::null) {
    CFBO& cfbo = registry.get<CFBO>(fbo);
    const auto& default_FBO = render_manager.get_framebuffers().at(cfbo.fbo);
    // go back to default framebuffer
    default_FBO->unbind();
    RenderAPI::clear_color();
    // disable depth test so screen-space quad isn't discarded due to depth test.
    RenderAPI::toggle_depth_test(false);
    CShape& cShape = registry.get<CShape>(fbo);
    cfbo.setup_properties(render_manager.get_shader_program("fbo"));
    const auto& settings_manager = app.get_settings_manager();
    if (settings_manager.imgui_window) {
      render_manager.render_inside_imgui(cShape.meshes.at(0)->get_vao(), cfbo.fbo, "scene", {0, 0}, {0, 0},
                                         settings_manager.fit_to_window);
    } else {
      render_manager.render_framebuffer(cShape.meshes.at(0)->get_vao(), cfbo.fbo);
    }
  }

  render_manager.end_scene();
}

}