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
#include "../../components/graphics/cMesh.h"
#include "../../components/graphics/cReflection.h"
#include "../../components/graphics/cShader.h"
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
            CBlendColor* cBlendColor, CReflection* cReflection, CSkybox* cSkybox, CTexture* cSkyboxTexture,
            CBlendTexture* cSkyboxBlend, CMesh* cMesh, const CTransform& cTransform, const CShader& cShader,
            CCollider* cCollider, CTransparent* cTransparent, RenderManager& render_manager) {
  const auto& application_manager = Application::get();
  if (cTransparent and cTransparent->transparent) {
    RenderAPI::toggle_culling(false);
  }
  if (cSkybox) {
    RenderAPI::set_depth_lequal();
  }
  // TODO refactor this to a system
  bool display_hitbox = false;
  if (cCollider) {
    display_hitbox = cCollider->display_hitbox or application_manager.get_settings_manager().display_collision_boxes;
  }
  // TODO im sending sp in a weird way here *application, reset uniform maybe i dont need?
  const auto& assets_manager = application_manager.get_assets_manager();
  AssetHandle<Shader> shader = assets_manager.get<Shader>(cShader.id);
  Shader& sp = *shader.get();
  cMesh->bind_textures(sp, cTexture, cBlendTexture, cTextureAtlas, cColor, cBlendColor, cReflection, cSkybox,
                       cSkyboxTexture, cSkyboxBlend);
  render_manager.render(cMesh->get_vao(), cTransform.calculate(), shader);
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
    sp.bind();
    sp.set_bool("display_hitbox", true);
    sp.set_vec4("hitbox_color", cCollider->color);
    sp.unbind();
    render_manager.render(cCollider->mesh.get_vao(), cTransform.calculate(), shader);
  } else if (cCollider and not display_hitbox) {
    sp.bind();
    sp.set_bool("display_hitbox", false);
    sp.unbind();
  }
}

void render_model(CBody* cBody, RenderManager& render_manager, const CTransform& cTransform) {
  const Model& model = *cBody->handle.get();
  for (const auto& submesh : model.get_submeshes()) {
    const Material& mat = *submesh.material.get();
    const RenderState& state = mat.get_state();
    const MaterialParams& params = mat.get_params();
    const MaterialTextures& textures = mat.get_textures();
    AssetHandle<Shader> shader = mat.get_shader_handle();
    Shader& sp = *shader.get();

    // if (state.blend)
    //   RenderAPI::toggle_blend(true);
    // if (!state.depth_write)
    //   RenderAPI::toggle_depth_write(false);
    if (!state.cull)
      RenderAPI::toggle_culling(false);

    // Set material uniforms while shader is bound.
    // render_manager.render() will re-bind and set MVP on top — that's fine,
    // OpenGL uniforms are per-program state and survive a re-bind.
    sp.bind();
    sp.set_vec4("u_base_color_factor", params.base_color_factor);
    sp.set_vec3("u_emissive_factor", params.emissive_factor);
    sp.set_float("u_metallic_factor", params.metallic_factor);
    sp.set_float("u_roughness_factor", params.roughness_factor);
    sp.set_float("u_alpha_cutoff", params.alpha_cutoff);

    uint32_t slot = 1;
    auto bind_tex = [&](const AssetHandle<Texture>& handle, std::string_view uniform) {
      if (handle.is_valid()) {
        handle.get()->bind_slot(slot);
        sp.set_int(uniform, static_cast<int>(slot++));
      }
    };
    // TODO only base color and normal map are accepted for now
    bind_tex(textures.base_color, "u_base_color");
    bind_tex(textures.normal, "u_normal_map");
    bind_tex(textures.metallic_roughness, "u_metallic_roughness");
    bind_tex(textures.emissive, "u_emissive_map");
    bind_tex(textures.occlusion, "u_occlusion_map");

    // render_manager binds shader again, sets projection/view/model/camera, draws, unbinds
    render_manager.render(submesh.mesh->get_vao(), cTransform.calculate(), shader);

    // if (state.blend)
    //   RenderAPI::toggle_blend(false);
    // if (!state.depth_write)
    //   RenderAPI::toggle_depth_write(true);
    if (!state.cull)
      RenderAPI::toggle_culling(true);
  }
}

void RenderSystem::update(entt::registry& registry, const Time& ts) {
  auto& app = Application::get();
  auto& render_manager = app.get_render_manager();

  // TODO: support more than one?
  entt::entity fbo = registry.view<CFBO, CUUID>().front();
  if (fbo not_eq entt::null) {
    const CFBO& cfbo = registry.get<CFBO>(fbo);
    const auto& default_FBO = render_manager.get_framebuffers().at(cfbo.id);
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

  registry.view<CTransform, CShader, CUUID>().each(
    [&](entt::entity e, const CTransform& cTransform, const CShader& cShader, const CUUID& cUUID) {
      // these two we will ignored for the entity sky
      CTexture* cTexture = registry.try_get<CTexture>(e);
      CBlendTexture* cBlendTexture = registry.try_get<CBlendTexture>(e);
      // not used by terrain or model
      CTextureAtlas* cTextureAtlas = registry.try_get<CTextureAtlas>(e);
      CColor* cColor = registry.try_get<CColor>(e);
      CBlendColor* cBlendColor = registry.try_get<CBlendColor>(e);
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

      if (cShader.visible) {
        if (cMesh) { // TODO objects with one mesh unused
          if (not cTexture and not cTextureAtlas) {
            CName* cName = registry.try_get<CName>(e);
            if (cName) {
              APP_ASSERT(false, "no texture found for entity {} {}", cUUID.uuid, cName->id);
            } else {
              APP_ASSERT(false, "no texture found for entity {}", cUUID.uuid);
            }
          }

          render(cTexture, cBlendTexture, cTextureAtlas, cColor, cBlendColor, cReflection, cSkybox, cSkyboxTexture,
                 cBlendTexture, cMesh, cTransform, cShader, cCollider, cTransparent, render_manager);
        } else if (cBody) { // models
          render_model(cBody, render_manager, cTransform);
        } else if (cShape) { // primitives
          if (not cTexture and not cTextureAtlas) {
            CName* cName = registry.try_get<CName>(e);
            if (cName) {
              APP_ASSERT(false, "no texture found for entity {} {}", cUUID.uuid, cName->id);
            } else {
              APP_ASSERT(false, "no texture found for entity {}", cUUID.uuid);
            }
          }

          for (auto& mesh : cShape->meshes) {
            render(cTexture, cBlendTexture, cTextureAtlas, cColor, cBlendColor, cReflection, cSkybox, cSkyboxTexture,
                   cBlendTexture, mesh.get(), cTransform, cShader, cCollider, cTransparent, render_manager);
          }
        } else {
          CName* cName = registry.try_get<CName>(e);
          if (cName) {
            APP_ASSERT(false, "no mesh found for entity {} {}", cUUID.uuid, cName->id);
          } else {
            APP_ASSERT(false, "no mesh found for entity {}", cUUID.uuid);
          }
        }
      }
    });

  if (fbo not_eq entt::null) {
    CFBO& cfbo = registry.get<CFBO>(fbo);
    const auto& default_FBO = render_manager.get_framebuffers().at(cfbo.id);
    // go back to default framebuffer
    default_FBO->unbind();
    RenderAPI::clear_color();
    // disable depth test so screen-space quad isn't discarded due to depth test.
    RenderAPI::toggle_depth_test(false);
    CShader& cShader = registry.get<CShader>(fbo);
    AssetHandle<Shader> shader = Application::get().get_assets_manager().get<Shader>(cShader.id);
    CShape& cShape = registry.get<CShape>(fbo);
    cfbo.setup_properties(shader);
    const auto& settings_manager = app.get_settings_manager();
    if (settings_manager.imgui_window) {
      render_manager.render_inside_imgui(cShape.meshes.at(0)->get_vao(), cfbo.id, "scene", {0, 0}, {0, 0},
                                         settings_manager.fit_to_window);
    } else {
      render_manager.render_framebuffer(cShape.meshes.at(0)->get_vao(), shader, cfbo.id);
    }
  }

  render_manager.end_scene();
}
}