#include "sRender.h"

#include <memory>

#include <glm/glm.hpp>

#include "../../application/application.h"
#include "../../components/camera/cActiveCamera.h"
#include "../../components/camera/cCamera.h"
#include "../../components/camera/cDistanceFromCamera.h"
#include "../../components/graphics/cBody.h"
#include "../../components/graphics/cFBO.h"
#include "../../components/graphics/cMaterial.h"
#include "../../components/graphics/cMesh.h"
#include "../../components/graphics/cShaderProgram.h"
#include "../../components/graphics/cShape.h"
#include "../../components/graphics/cTexture.h"
#include "../../components/graphics/cTextureAtlas.h"
#include "../../components/meta/cName.h"
#include "../../components/meta/cUUID.h"
#include "../../components/physics/cCollider.h"
#include "../../components/physics/cTransform.h"
#include "../../components/world/cSkybox.h"
#include "../../graphics/render_api.h"
#include "../../logging/log_manager.h"
#include "../../settings/settings_manager.h"

namespace nl {

void render(CTexture* cTexture, CTextureAtlas* cTextureAtlas,
            const CSkybox* cSkybox, CMaterial* cMaterial, CMesh* cMesh,
            const CTransform& cTransform, const CShaderProgram& cShaderProgram,
            CTexture* cSkyboxTexture, CCollider* cCollider,
            const std::unique_ptr<RenderManager>& render_manager) {
  if (cTexture and cTexture->enable_transparency) {
    RenderAPI::toggle_culling(false);
  }
  if (cSkybox) {
    RenderAPI::set_depth_lequal();
  }
  cMesh->bind_textures(render_manager->get_shader_program(cShaderProgram.name),
                       cTexture, cTextureAtlas, cSkyboxTexture, cMaterial);
  render_manager->render(cMesh->get_vao(), cTransform.calculate(),
                         cShaderProgram.name);
  cMesh->unbind_textures(cTexture);
  if (cTexture and cTexture->enable_transparency) {
    RenderAPI::toggle_culling(true);
  }
  if (cSkybox) {
    RenderAPI::set_depth_less();
  }
  if (cCollider and
      Application::get().get_settings_manager()->display_collision_boxes) {
    // TODO fix transparency so I can render this first
    // disabling culling is not working
    const auto& sp = render_manager->get_shader_program("shape");
    sp->reset_active_uniforms();
    sp->use();
    sp->set_float("use_color", 1.f);
    sp->set_vec4("color", cCollider->color);
    sp->unuse();
    render_manager->render(cCollider->mesh.get_vao(), cTransform.calculate(),
                           "shape");
  }
}

void RenderSystem::update(entt::registry& registry, const Time& ts) {
  auto& app = Application::get();
  const auto& render_manager = app.get_render_manager();

  entt::entity fbo =
    registry.view<CFBO, CUUID>().front(); // TODO: support more than one?
  if (fbo not_eq entt::null) {
    CFBO& cfbo = registry.get<CFBO>(fbo);
    const auto& default_FBO = render_manager->get_framebuffers().at(cfbo.fbo);
    default_FBO->bind_to_draw();
    RenderAPI::toggle_depth_test(true);
  }

  // FBOs are cleared in their own render pass at the end of the scene
  RenderAPI::clear();
  render_manager->reset_metrics();

  entt::entity camera =
    registry.view<CCamera, CActiveCamera, CTransform, CUUID>().front();
  APP_ASSERT(camera not_eq entt::null, "no active camera found!");
  CCamera& cCamera = registry.get<CCamera>(camera);
  const CTransform& cCameraTransform = registry.get<CTransform>(camera);
  cCamera.calculate_view(cCameraTransform.position, cCameraTransform.rotation);
  render_manager->begin_scene(cCamera.view, cCamera.projection,
                              cCameraTransform.position);

  entt::entity sky =
    registry.view<CSkybox, CUUID>().front(); // TODO: support more than one?
  CTexture* cSkyboxTexture;
  if (sky not_eq entt::null) {
    cSkyboxTexture = registry.try_get<CTexture>(sky);
  }

  if (render_manager->should_reorder()) {
    registry.sort<CDistanceFromCamera>(
      [](const CDistanceFromCamera& lhs, const CDistanceFromCamera& rhs) {
        return lhs.distance < rhs.distance;
      });
    registry.sort<CUUID, CDistanceFromCamera>();
  }

  registry.view<CTransform, CShaderProgram, CUUID>().each(
    [&](entt::entity e, const CTransform& cTransform,
        const CShaderProgram& cShaderProgram, const CUUID& cUUID) {
      CTexture* cTexture = registry.try_get<CTexture>(e);
      CTextureAtlas* cTextureAtlas = registry.try_get<CTextureAtlas>(e);
      CSkybox* cSkybox = registry.try_get<CSkybox>(e);
      CMaterial* cMaterial = registry.try_get<CMaterial>(e);
      CBody* cBody = registry.try_get<CBody>(e);
      CMesh* cMesh = registry.try_get<CMesh>(e);
      CShape* cShape = registry.try_get<CShape>(e);
      CCollider* cCollider = registry.try_get<CCollider>(e);

      if (cShaderProgram.visible) {
        if (cMesh) { // TODO objects with one mesh unused
          if (not cTexture) {
            CName* cName = registry.try_get<CName>(e);
            if (cName) {
              APP_ASSERT(false, "no texture found for entity {} {}", cUUID.uuid,
                         cName->name);
            } else {
              APP_ASSERT(false, "no texture found for entity {}", cUUID.uuid);
            }
          }

          render(cTexture, cTextureAtlas, cSkybox, cMaterial, cMesh, cTransform,
                 cShaderProgram, cSkyboxTexture, cCollider, render_manager);
        } else if (cBody) { // models
          for (uint32_t i = 0; i < cBody->meshes.size(); ++i) {
            CMesh& mesh = cBody->meshes.at(i);
            CMaterial& material = cBody->materials.at(i);
            render(cTexture, cTextureAtlas, cSkybox, &material, &mesh,
                   cTransform, cShaderProgram, cSkyboxTexture, cCollider,
                   render_manager);
          }
        } else if (cShape) { // primitives
          if (not cTexture) {
            CName* cName = registry.try_get<CName>(e);
            if (cName) {
              APP_ASSERT(false, "no texture found for entity {} {}", cUUID.uuid,
                         cName->name);
            } else {
              APP_ASSERT(false, "no texture found for entity {}", cUUID.uuid);
            }
          }

          for (auto& mesh : cShape->meshes) {
            render(cTexture, cTextureAtlas, cSkybox, cMaterial, &mesh,
                   cTransform, cShaderProgram, cSkyboxTexture, cCollider,
                   render_manager);
          }
        } else {
          CName* cName = registry.try_get<CName>(e);
          if (cName) {
            APP_ASSERT(false, "no mesh found for entity {} {}", cUUID.uuid,
                       cName->name);
          } else {
            APP_ASSERT(false, "no mesh found for entity {}", cUUID.uuid);
          }
        }
      }
    });

  if (fbo not_eq entt::null) {
    CFBO& cfbo = registry.get<CFBO>(fbo);
    const auto& default_FBO = render_manager->get_framebuffers().at(cfbo.fbo);
    default_FBO->unbind(); // go back to default framebuffer
    RenderAPI::clear_color();
    RenderAPI::toggle_depth_test(
      false); // disable depth test so screen-space quad isn't discarded due to
              // depth test.
    CShape& cShape = registry.get<CShape>(fbo);
    cfbo.setup_properties(render_manager->get_shader_program("fbo"));
    const auto& settings_manager = app.get_settings_manager();
    if (settings_manager->imgui_window) {
      render_manager->render_inside_imgui(cShape.meshes.at(0).get_vao(),
                                          cfbo.fbo, "scene", {0, 0}, {0, 0},
                                          settings_manager->fit_to_window);
    } else {
      render_manager->render_framebuffer(cShape.meshes.at(0).get_vao(),
                                         cfbo.fbo);
    }
  }

  render_manager->end_scene();
}

}