#include "sRender.h"

#include <memory>

#include <glm/glm.hpp>

#include "../../application/application.h"
#include "../../components/camera/cActiveCamera.h"
#include "../../components/camera/cCamera.h"
#include "../../components/camera/cDistanceFromCamera.h"
#include "../../components/meta/cName.h"
#include "../../components/meta/cUUID.h"
#include "../../components/physics/cCollider.h"
#include "../../components/physics/cTransform.h"
#include "../../components/render/CModel.h"
#include "../../components/render/cBlendColor.h"
#include "../../components/render/cBlendTexture.h"
#include "../../components/render/cColor.h"
#include "../../components/render/cFBO.h"
#include "../../components/render/cReflection.h"
#include "../../components/render/cShader.h"
#include "../../components/render/cShape.h"
#include "../../components/render/cTexture.h"
#include "../../components/render/cTextureAtlas.h"
#include "../../components/render/cTransparent.h"
#include "../../components/world/cSkybox.h"
#include "../../logging/log_manager.h"
#include "../../render/render_api.h"
#include "../../settings/settings_manager.h"

namespace nl {

// TODO maybe avoid setting uniform at all if disable, check after refactor and removing monostate
// TODO rethink with uniform buffer object in system
void configure_fog(Shader& sp) {
  sp.set_bool("fog_enabled", static_cast<bool>(entt::monostate<"fog_enabled"_hs>{}));
  sp.set_vec4("fog_color", static_cast<glm::vec4>(entt::monostate<"fog_color"_hs>{}));
  sp.set_float("fog_density", static_cast<float>(entt::monostate<"fog_density"_hs>{}));
  sp.set_float("fog_gradient", static_cast<float>(entt::monostate<"fog_gradient"_hs>{}));
  sp.set_float("fog_lower_limit", static_cast<float>(entt::monostate<"fog_lower_limit"_hs>{}));
  sp.set_float("fog_upper_limit", static_cast<float>(entt::monostate<"fog_upper_limit"_hs>{}));
}

// TODO rethink with uniform buffer object in system
void configure_light(Shader& sp) {
  sp.set_bool("light_enabled", static_cast<bool>(entt::monostate<"light_enabled"_hs>{}));
  sp.set_vec3("light_color", static_cast<glm::vec3>(entt::monostate<"light_color"_hs>{}));
  sp.set_vec3("light_position", static_cast<glm::vec3>(entt::monostate<"light_position"_hs>{}));
  sp.set_float("light_intensity", static_cast<float>(entt::monostate<"light_intensity"_hs>{}));
  sp.set_float("light_range", static_cast<float>(entt::monostate<"light_range"_hs>{}));
  sp.set_float("light_inner_cone_angle", static_cast<float>(entt::monostate<"light_inner_cone_angle"_hs>{}));
  sp.set_float("light_outer_cone_angle", static_cast<float>(entt::monostate<"light_outer_cone_angle"_hs>{}));
}

void configure_reflected_skybox(Shader& sp, CTexture* cSkyboxTexture, CBlendTexture* cSkyboxBlend,
                                uint32_t cubemap_sampler) {
  // TODO better reflection than sending the skybox texture and the blend
  // that's why we send them to all shaders
  // TODO use uniform buffer object for this and think a better way to avoid sky entity that checking shader name
  if (cSkyboxTexture and sp.get_uuid() not_eq "skybox") {
    // TODO fix literals for texture slots
    // 10 and 11 are reserved for skybox
    sp.set_int("skybox_texture", 10);
    auto skybox_texture = cSkyboxTexture->handle.get();
    if (skybox_texture) {
      skybox_texture->bind(10);
      glBindSampler(10, cubemap_sampler);
    }
    if (cSkyboxBlend) {
      sp.set_bool("blend_skybox_enabled", true);
      sp.set_int("blend_skybox_texture", 11);
      if (auto blend_texture = cSkyboxBlend->handle.get()) {
        blend_texture->bind(11);
        glBindSampler(11, cubemap_sampler);
      }
      sp.set_float("blend_skybox_factor", cSkyboxBlend->blend_factor);
    } else {
      sp.set_bool("blend_skybox_enabled", false);
    }
  }
}

void configure_reflection(Shader& sp, CReflection* cReflection, CTexture* cSkyboxTexture, CBlendTexture* cSkyboxBlend,
                          uint32_t cubemap_sampler) {
  if (cReflection) {
    sp.set_bool("reflection_enabled", cReflection->enabled);
    sp.set_float("reflectivity", cReflection->reflectivity);
    sp.set_float("refractivity", cReflection->refractivity);
    configure_reflected_skybox(sp, cSkyboxTexture, cSkyboxBlend, cubemap_sampler);
  }
}

void configure_color(Shader& sp, CColor* cColor) {
  if (cColor) {
    sp.set_bool("color_enabled", true);
    sp.set_vec4("color", cColor->color);
  } else {
    sp.set_bool("color_enabled", false);
  }
}

void configure_blend(Shader& sp, CBlendTexture* cBlendTexture, CBlendColor* cBlendColor) {
  if (cBlendTexture) {
    sp.set_bool("blend_texture_enabled", true);
    sp.set_float("blend_texture_factor", cBlendTexture->blend_factor);
    sp.set_int("blend_texture", 9); // slot 9 reserved for blend texture
    if (auto blend_texture = cBlendTexture->handle.get()) {
      blend_texture->bind(9);
    }
  } else {
    sp.set_bool("blend_texture_enabled", false);
  }
  if (cBlendColor) {
    sp.set_bool("blend_color_enabled", true);
    sp.set_float("blend_color_factor", cBlendColor->blend_factor);
    sp.set_vec4("blend_color", cBlendColor->color);
  } else {
    sp.set_bool("blend_color_enabled", false);
  }
}

// TODO move to system and rethink with uniform buffer object in system
void configure_texture_atlas(Shader& sp, CTextureAtlas* cTextureAtlas) {
  // TODO terrain shader not using this logic at all (get from terrain vertex directly)
  if (cTextureAtlas) {
    auto atlas_texture = cTextureAtlas->handle.get();
    if (!atlas_texture) {
      return;
    }
    // FIXME remove text parameteri from texture and have here a default one
    atlas_texture->bind(1);
    uint32_t index = cTextureAtlas->index;
    uint32_t rows = cTextureAtlas->rows;
    sp.set_float("texture_atlas_rows", rows);
    uint32_t col = index % rows;
    float coll = static_cast<float>(col) / rows;
    uint32_t row = index / rows;
    float roww = static_cast<float>(row) / rows;
    sp.set_vec2("texture_atlas_offset", glm::vec2(coll, roww));
  }
}

// TODO remove probably
void configure_texture(Shader& sp, CTexture* cTexture) {
  if (cTexture) {
    auto texture = cTexture->handle.get();
    ENGINE_ASSERT(texture, "invalid texture handle for entity with uuid {}", cTexture->uuid);
    texture->bind(0);
  }
}

// TODO remove this and rethink in systems with uniform buffer objects
void bind_textures(Shader& sp, CTexture* cTexture, CBlendTexture* cBlendTexture, CTextureAtlas* cTextureAtlas,
                   CColor* cColor, CBlendColor* cBlendColor, CReflection* cReflection, CSkybox* cSkybox,
                   CTexture* cSkyboxTexture, CBlendTexture* cSkyboxBlend, uint32_t cubemap_sampler) {
  sp.bind();
  configure_fog(sp);
  configure_light(sp);
  configure_reflection(sp, cReflection, cSkyboxTexture, cSkyboxBlend, cubemap_sampler);
  // TODO add model textures to CTexture and remove this and use only one shader maybe
  configure_texture(sp, cTexture);
  configure_texture_atlas(sp, cTextureAtlas);
  configure_color(sp, cColor);
  configure_blend(sp, cBlendTexture, cBlendColor);
}

// TODO refactor this or delete
void render(CTexture* cTexture, CBlendTexture* cBlendTexture, CTextureAtlas* cTextureAtlas, CColor* cColor,
            CBlendColor* cBlendColor, CReflection* cReflection, CSkybox* cSkybox, CTexture* cSkyboxTexture,
            CBlendTexture* cSkyboxBlend, CModel* cModel, const CTransform& cTransform, const CShader& cShader,
            CCollider* cCollider, CTransparent* cTransparent, RenderManager& render_manager, uint32_t cubemap_sampler) {
  const auto& application_manager = Application::get();
  if (cTransparent and cTransparent->transparent) {
    RenderAPI::set_culling(false);
  }
  if (cSkybox) {
    // TODO is this needeed for skybox?
    RenderAPI::set_depth_func(RenderAPI::DepthFunc::LessEqual);
  }
  // TODO refactor this to a system
  bool display_hitbox = false;
  if (cCollider) {
    display_hitbox = cCollider->display_hitbox or application_manager.get_settings_manager().display_collision_boxes;
  }
  // TODO im sending sp in a weird way here *application, reset uniform maybe i dont need?
  const auto& assets_manager = application_manager.get_assets_manager();
  AssetHandle<Shader> shader = cShader.handle;
  Shader& sp = *shader.get();
  bind_textures(sp, cTexture, cBlendTexture, cTextureAtlas, cColor, cBlendColor, cReflection, cSkybox, cSkyboxTexture,
                cSkyboxBlend, cubemap_sampler);
  // FIXME render_manager.render(cModel.->get_vao(), cTransform.calculate(), shader);
  if (cTransparent and cTransparent->transparent) {
    RenderAPI::set_culling(true);
  }
  if (cSkybox) {
    RenderAPI::set_depth_func(RenderAPI::DepthFunc::LessEqual);
  }
  if (cCollider and display_hitbox) {
    // TODO fix transparency so I can render this first
    // disabling culling is not working
    // this can't be in Mesh bind_textures right now
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

// Sets default sampler parameters for all materials that don't specify their own sampler.
// This is separate from the Texture class because some materials might want different sampler settings (e.g. clamp vs
// repeat).
void RenderSystem::setup_default_sampler() {
  glCreateSamplers(1, &m_default_sampler);
  glSamplerParameteri(m_default_sampler, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glSamplerParameteri(m_default_sampler, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glSamplerParameteri(m_default_sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glSamplerParameteri(m_default_sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
#ifdef GL_EXT_texture_filter_anisotropic
  float maxAniso = 0.0f;
  glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAniso);
  if (maxAniso > 0.0f) {
    glSamplerParameterf(m_default_sampler, GL_TEXTURE_MAX_ANISOTROPY_EXT, (std::min)(4.0f, maxAniso));
  }
#endif
  glObjectLabel(GL_SAMPLER, m_default_sampler, -1, "DefaultSampler");
}

void RenderSystem::setup_cubemap_sampler() {
  glCreateSamplers(1, &m_cubemap_sampler);
  glSamplerParameteri(m_cubemap_sampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glSamplerParameteri(m_cubemap_sampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glSamplerParameteri(m_cubemap_sampler, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  glSamplerParameteri(m_cubemap_sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glSamplerParameteri(m_cubemap_sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glObjectLabel(GL_SAMPLER, m_cubemap_sampler, -1, "CubemapSampler");
}

void render_model(CModel* cModel, RenderManager& render_manager, const CTransform& cTransform,
                  uint32_t default_sampler) {
  const Model& model = *cModel->handle.get();
  for (const auto& submesh : model.get_submeshes()) {
    const Material& mat = *submesh.material.get();
    const RenderState& state = mat.get_state();
    const MaterialParams& params = mat.get_params();
    const MaterialTextures& textures = mat.get_textures();
    AssetHandle<Shader> shader = mat.get_shader_handle();
    Shader& sp = *shader.get();

    if (!state.cull)
      RenderAPI::set_culling(false);

    // TODO opaque and transparent pass
    // if (state.blend)
    //   RenderAPI::set_blend(true);
    // if (!state.depth_write)
    //   RenderAPI::set_depth_mask(false);

    sp.bind();
    sp.set_vec4("u_base_color_factor", params.base_color_factor);
    sp.set_vec3("u_emissive_factor", params.emissive_factor);
    sp.set_float("u_metallic_factor", params.metallic_factor);
    sp.set_float("u_roughness_factor", params.roughness_factor);
    sp.set_float("u_alpha_cutoff", params.alpha_cutoff);

    uint32_t slot = 0;
    auto bind_texture = [&](const AssetHandle<Texture>& handle, std::string_view uniform) {
      if (handle.is_valid()) {
        handle.get()->bind(slot);
        glBindSampler(slot, default_sampler);
        // TODO fix literals for texture slots using the shader?
        sp.set_int(uniform, static_cast<int>(slot));
        ++slot;
      }
    };

    // TODO add fallback
    bind_texture(textures.base_color, "u_base_color");
    bind_texture(textures.normal, "u_normal_map");
    bind_texture(textures.metallic_roughness, "u_metallic_roughness");
    bind_texture(textures.emissive, "u_emissive_map");
    bind_texture(textures.occlusion, "u_occlusion_map");

    // should i remove the logic from that class?
    render_manager.render(submesh.mesh->get_vao(), cTransform.calculate(), shader);

    if (!state.cull)
      RenderAPI::set_culling(true);

    // TODO opaque and transparent pass
    if (state.blend)
      RenderAPI::set_blend(false);
    if (!state.depth_write)
      RenderAPI::set_depth_mask(true);
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
    RenderAPI::set_depth_test(true);
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
      CModel* cModel = registry.try_get<CModel>(e);
      CShape* cShape = registry.try_get<CShape>(e);
      CCollider* cCollider = registry.try_get<CCollider>(e);

      if (cShader.visible) {
        if (cModel) { // models
          // TODO this should or call different renderers or use like this but then remove texture bind method
          // and shape should be using the same,not sure about the comment i have done of
          // adding textures from model to CTexture
          render_model(cModel, render_manager, cTransform, m_default_sampler);
        } else if (cShape) { // primitives
          // TODO rethink with new model class and mesh class
          // if (not cTexture and not cTextureAtlas) {
          //   CName* cName = registry.try_get<CName>(e);
          //   if (cName) {
          //     APP_ASSERT(false, "no texture found for entity {} {}", cUUID.uuid, cName->id);
          //   } else {
          //     APP_ASSERT(false, "no texture found for entity {}", cUUID.uuid);
          //   }
          // }

          // for (auto& mesh : cShape->meshes) {
          //   render(cTexture, cBlendTexture, cTextureAtlas, cColor, cBlendColor, cReflection, cSkybox, cSkyboxTexture,
          //          cBlendTexture, mesh.get(), cTransform, cShader, cCollider, cTransparent, render_manager);
          // }
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
    // TODO remove after adding SSBO for fbo instead of using a quad, check if quad work for mirrors
    // disable depth test so screen-space quad isn't discarded due to depth test.
    RenderAPI::set_depth_test(false);
    CShader& cShader = registry.get<CShader>(fbo);
    CShape& cShape = registry.get<CShape>(fbo);
    cfbo.setup_properties(cShader.handle);
    const auto& settings_manager = app.get_settings_manager();
    if (settings_manager.imgui_window) {
      render_manager.render_inside_imgui(cShape.meshes.at(0)->get_vao(), cfbo.id, "scene", {0, 0}, {0, 0},
                                         settings_manager.fit_to_window);
    } else {
      render_manager.render_framebuffer(cShape.meshes.at(0)->get_vao(), cShader.handle, cfbo.id);
    }
  }

  render_manager.end_scene();
}

}