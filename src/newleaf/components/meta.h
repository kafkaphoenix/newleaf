#pragma once

#include <entt/entt.hpp>

#include "../utils/assert.h"
#include "camera/cActiveCamera.h"
#include "camera/cCamera.h"
#include "camera/cDistanceFromCamera.h"
#include "meta/cName.h"
#include "meta/cTag.h"
#include "meta/cUUID.h"
#include "graphics/cBody.h"
#include "graphics/cFBO.h"
#include "graphics/cMaterial.h"
#include "graphics/cMesh.h"
#include "graphics/cShaderProgram.h"
#include "graphics/cShape.h"
#include "graphics/cTexture.h"
#include "graphics/cTextureAtlas.h"
#include "input/cActiveInput.h"
#include "input/cInput.h"
#include "physics/cCollider.h"
#include "physics/cGravity.h"
#include "physics/cRigidBody.h"
#include "physics/cTransform.h"
#include "world/cLight.h"
#include "world/cSkybox.h"
#include "world/cTime.h"

using namespace entt::literals;

namespace nl::components {

template <typename Component, typename... Args>
inline Component& assign(entt::entity e, Args... args) {
  auto& registry = Application::Get().get_scene_manager()->get_registry();
  ENGINE_ASSERT(not registry.all_of<Component>(e),
                "entity already has component {}", typeid(Component).name());
  return registry.emplace<Component>(e, std::forward<Args>(args)...);
}

template <typename Component>
inline Component& on_component_added(entt::entity e, Component& c) {
  Application::Get().get_scene_manager()->on_component_added<Component>(e, c);
  return c;
}

template <typename Component>
inline Component& on_component_cloned(entt::entity e, Component& c) {
  Application::Get().get_scene_manager()->on_component_cloned<Component>(e, c);
  return c;
}

////////////////////////////////////////////////////////////////////////////////////////

CActiveCamera& cast_cactive_camera(void* other) {
  return *static_cast<CActiveCamera*>(other);
}

CCamera& cast_ccamera(void* other) { return *static_cast<CCamera*>(other); }

CDistanceFromCamera& cast_cdistance_from_camera(void* other) {
  return *static_cast<CDistanceFromCamera*>(other);
}

CName& cast_cname(void* other) { return *static_cast<CName*>(other); }

CTag& cast_ctag(void* other) { return *static_cast<CTag*>(other); }

CUUID& cast_cuuid(void* other) { return *static_cast<CUUID*>(other); }

CBody& cast_cbody(void* other) { return *static_cast<CBody*>(other); }

CFBO& cast_cfbo(void* other) { return *static_cast<CFBO*>(other); }

CMaterial& cast_cmaterial(void* other) {
  return *static_cast<CMaterial*>(other);
}

CMesh& cast_cmesh(void* other) { return *static_cast<CMesh*>(other); }

CShaderProgram& cast_cshader_program(void* other) {
  return *static_cast<CShaderProgram*>(other);
}

CShape& cast_cshape(void* other) { return *static_cast<CShape*>(other); }

CTexture& cast_ctexture(void* other) { return *static_cast<CTexture*>(other); }

CTextureAtlas& cast_ctexture_atlas(void* other) {
  return *static_cast<CTextureAtlas*>(other);
}

CActiveInput& cast_cactive_input(void* other) {
  return *static_cast<CActiveInput*>(other);
}

CInput& cast_cinput(void* other) { return *static_cast<CInput*>(other); }

CCollider& cast_ccollider(void* other) {
  return *static_cast<CCollider*>(other);
}

CGravity& cast_cgravity(void* other) { return *static_cast<CGravity*>(other); }

CRigidBody& cast_crigid_body(void* other) {
  return *static_cast<CRigidBody*>(other);
}

CTransform& cast_ctransform(void* other) {
  return *static_cast<CTransform*>(other);
}

CLight& cast_clight(void* other) { return *static_cast<CLight*>(other); }

CSkybox& cast_cskybox(void* other) { return *static_cast<CSkybox*>(other); }

CTime& cast_ctime(void* other) { return *static_cast<CTime*>(other); }

////////////////////////////////////////////////////////////////////////////////////////

void register_components() {
  entt::meta<CActiveCamera>()
    .type("active_camera"_hs)
    .ctor<&cast_cactive_camera, entt::as_ref_t>()
    .func<&CActiveCamera::print>("print"_hs)
    .func<&CActiveCamera::to_map>("to_map"_hs)
    .func<&assign<CActiveCamera>, entt::as_ref_t>("assign"_hs);

  entt::meta<CCamera>()
    .type("camera"_hs)
    .ctor<&cast_ccamera, entt::as_ref_t>()
    .data<&CCamera::_type>("type"_hs)
    .data<&CCamera::_aspect_ratio>("aspect_ratio"_hs)
    .data<&CCamera::_mode>("mode"_hs)
    .data<&CCamera::fov>("fov"_hs)
    .data<&CCamera::zoom_factor>("zoom_factor"_hs)
    .data<&CCamera::zoom_min>("zoom_min"_hs)
    .data<&CCamera::zoom_max>("zoom_max"_hs)
    .data<&CCamera::near_clip>("near_clip"_hs)
    .data<&CCamera::far_clip>("far_clip"_hs)
    .func<&CCamera::print>("print"_hs)
    .func<&CCamera::to_map>("to_map"_hs)
    .func<&on_component_added<CCamera>, entt::as_ref_t>("on_component_added"_hs)
    .func<&assign<CCamera>, entt::as_ref_t>("assign"_hs);

  entt::meta<CDistanceFromCamera>()
    .type("distance_from_camera"_hs)
    .ctor<&cast_cdistance_from_camera, entt::as_ref_t>()
    .data<&CDistanceFromCamera::distance>("distance"_hs)
    .func<&CDistanceFromCamera::print>("print"_hs)
    .func<&CDistanceFromCamera::to_map>("to_map"_hs)
    .func<&assign<CDistanceFromCamera>, entt::as_ref_t>("assign"_hs);

  entt::meta<CName>()
    .type("name"_hs)
    .ctor<&cast_cname, entt::as_ref_t>()
    .data<&CName::name>("name"_hs)
    .func<&CName::print>("print"_hs)
    .func<&CName::to_map>("to_map"_hs)
    .func<&assign<CName, std::string>, entt::as_ref_t>("assign"_hs);

  entt::meta<CTag>()
    .type("tag"_hs)
    .ctor<&cast_ctag, entt::as_ref_t>()
    .data<&CTag::tag>("tag"_hs)
    .func<&CTag::print>("print"_hs)
    .func<&CTag::to_map>("to_map"_hs)
    .func<&assign<CTag, std::string>, entt::as_ref_t>("assign"_hs);

  entt::meta<CUUID>()
    .type("uuid"_hs)
    .ctor<&cast_cuuid, entt::as_ref_t>()
    .data<&CUUID::uuid>("uuid"_hs)
    .func<&CUUID::print>("print"_hs)
    .func<&CUUID::to_map>("to_map"_hs);

  entt::meta<CBody>()
    .type("body"_hs)
    .ctor<&cast_cbody, entt::as_ref_t>()
    .data<&CBody::path>("path"_hs)
    .data<&CBody::meshes>("meshes"_hs)
    .data<&CBody::materials>("materials"_hs)
    .func<&CBody::print>("print"_hs)
    .func<&CBody::to_map>("to_map"_hs)
    .func<&on_component_added<CBody>, entt::as_ref_t>("on_component_added"_hs)
    .func<&assign<CBody, std::string>, entt::as_ref_t>("assign"_hs);

  entt::meta<CFBO>()
    .type("fbo"_hs)
    .ctor<&cast_cfbo, entt::as_ref_t>()
    .data<&CFBO::fbo>("fbo"_hs)
    .data<&CFBO::_mode>("mode"_hs)
    .data<&CFBO::_attachment>("attachment"_hs)
    .data<&CFBO::width>("width"_hs)
    .data<&CFBO::height>("height"_hs)
    .func<&CFBO::print>("print"_hs)
    .func<&CFBO::to_map>("to_map"_hs)
    .func<&on_component_added<CFBO>, entt::as_ref_t>("on_component_added"_hs)
    .func<&assign<CFBO>, entt::as_ref_t>("assign"_hs);

  entt::meta<CMaterial>()
    .type("material"_hs)
    .ctor<&cast_cmaterial, entt::as_ref_t>()
    .data<&CMaterial::ambient>("ambient"_hs)
    .data<&CMaterial::diffuse>("diffuse"_hs)
    .data<&CMaterial::specular>("specular"_hs)
    .data<&CMaterial::shininess>("shininess"_hs)
    .func<&CMaterial::print>("print"_hs)
    .func<&CMaterial::to_map>("to_map"_hs)
    .func<&assign<CMaterial>, entt::as_ref_t>("assign"_hs);

  entt::meta<CMesh>()
    .type("mesh"_hs)
    .ctor<&cast_cmesh, entt::as_ref_t>()
    .data<&CMesh::vertices>("vertices"_hs)
    .data<&CMesh::indices>("indices"_hs)
    .data<&CMesh::textures>("textures"_hs)
    .data<&CMesh::vao>("vao"_hs)
    .data<&CMesh::vertex_type>("vertex_type"_hs)
    .func<&CMesh::print>("print"_hs)
    .func<&CMesh::to_map>("to_map"_hs)
    .func<&assign<CMesh>, entt::as_ref_t>("assign"_hs);

  entt::meta<CShaderProgram>()
    .type("shader_program"_hs)
    .ctor<&cast_cshader_program, entt::as_ref_t>()
    .data<&CShaderProgram::name>("name"_hs)
    .data<&CShaderProgram::visible>("visible"_hs)
    .func<&CShaderProgram::print>("print"_hs)
    .func<&CShaderProgram::to_map>("to_map"_hs)
    .func<&assign<CShaderProgram>, entt::as_ref_t>("assign"_hs);

  entt::meta<CShape>()
    .type("shape"_hs)
    .ctor<&cast_cshape, entt::as_ref_t>()
    .data<&CShape::_type>("type"_hs)
    .data<&CShape::size>("size"_hs)
    .data<&CShape::meshes>("meshes"_hs)
    .data<&CShape::repeat_texture>("repeat_texture"_hs)
    .func<&CShape::print>("print"_hs)
    .func<&CShape::to_map>("to_map"_hs)
    .func<&on_component_added<CShape>, entt::as_ref_t>("on_component_added"_hs)
    .func<&assign<CShape>, entt::as_ref_t>("assign"_hs);

  entt::meta<CTexture>()
    .type("texture"_hs)
    .ctor<&cast_ctexture, entt::as_ref_t>()
    .data<&CTexture::paths>("paths"_hs)
    .data<&CTexture::textures>("textures"_hs)
    .data<&CTexture::color>("color"_hs)
    .data<&CTexture::blend_factor>("blend_factor"_hs)
    .data<&CTexture::reflectivity>("reflectivity"_hs)
    .data<&CTexture::refractive_index>("refractive_index"_hs)
    .data<&CTexture::enable_transparency>("enable_transparency"_hs)
    .data<&CTexture::enable_lighting>("enable_lighting"_hs)
    .data<&CTexture::enable_reflection>("enable_reflection"_hs)
    .data<&CTexture::enable_refraction>("enable_refraction"_hs)
    .data<&CTexture::_draw_mode>("draw_mode"_hs)
    .func<&CTexture::print>("print"_hs)
    .func<&CTexture::to_map>("to_map"_hs)
    .func<&on_component_added<CTexture>, entt::as_ref_t>(
      "on_component_added"_hs)
    .func<&assign<CTexture>, entt::as_ref_t>("assign"_hs);

  entt::meta<CTextureAtlas>()
    .type("texture_atlas"_hs)
    .ctor<&cast_ctexture_atlas, entt::as_ref_t>()
    .data<&CTextureAtlas::rows>("rows"_hs)
    .data<&CTextureAtlas::index>("index"_hs)
    .func<&CTextureAtlas::print>("print"_hs)
    .func<&CTextureAtlas::to_map>("to_map"_hs)
    .func<&assign<CTextureAtlas>, entt::as_ref_t>("assign"_hs);

  entt::meta<CActiveInput>()
    .type("active_input"_hs)
    .ctor<&cast_cactive_input, entt::as_ref_t>()
    .func<&CActiveInput::print>("print"_hs)
    .func<&CActiveInput::to_map>("to_map"_hs)
    .func<&assign<CActiveInput>, entt::as_ref_t>("assign"_hs);

  entt::meta<CInput>()
    .type("input"_hs)
    .ctor<&cast_cinput, entt::as_ref_t>()
    .data<&CInput::_mode>("mode"_hs)
    .data<&CInput::mouse_sensitivity>("mouse_sensitivity"_hs)
    .data<&CInput::translation_speed>("translation_speed"_hs)
    .data<&CInput::vertical_speed>("vertical_speed"_hs)
    .data<&CInput::rotation_speed>("rotation_speed"_hs)
    .func<&CInput::print>("print"_hs)
    .func<&CInput::to_map>("to_map"_hs)
    .func<&on_component_added<CInput>, entt::as_ref_t>("on_component_added"_hs)
    .func<&assign<CInput>, entt::as_ref_t>("assign"_hs);

  entt::meta<CCollider>()
    .type("collider"_hs)
    .ctor<&cast_ccollider, entt::as_ref_t>()
    .data<&CCollider::_type>("type"_hs)
    .data<&CCollider::size>("size"_hs)
    .func<&CCollider::print>("print"_hs)
    .func<&CCollider::to_map>("to_map"_hs)
    .func<&on_component_added<CCollider>, entt::as_ref_t>(
      "on_component_added"_hs)
    .func<&assign<CCollider>, entt::as_ref_t>("assign"_hs);

  entt::meta<CGravity>()
    .type("gravity"_hs)
    .ctor<&cast_cgravity, entt::as_ref_t>()
    .data<&CGravity::acceleration>("acceleration"_hs)
    .func<&CGravity::print>("print"_hs)
    .func<&CGravity::to_map>("to_map"_hs)
    .func<&assign<CGravity>, entt::as_ref_t>("assign"_hs);

  entt::meta<CRigidBody>()
    .type("rigid_body"_hs)
    .ctor<&cast_crigid_body, entt::as_ref_t>()
    .data<&CRigidBody::mass>("mass"_hs)
    .data<&CRigidBody::friction>("friction"_hs)
    .data<&CRigidBody::bounciness>("bounciness"_hs)
    .data<&CRigidBody::kinematic>("kinematic"_hs)
    .func<&CRigidBody::print>("print"_hs)
    .func<&CRigidBody::to_map>("to_map"_hs)
    .func<&assign<CRigidBody>, entt::as_ref_t>("assign"_hs);

  entt::meta<CTransform>()
    .type("transform"_hs)
    .ctor<&cast_ctransform, entt::as_ref_t>()
    .data<&CTransform::position>("position"_hs)
    .data<&CTransform::rotation>("rotation"_hs)
    .data<&CTransform::scale>("scale"_hs)
    .func<&CTransform::print>("print"_hs)
    .func<&CTransform::to_map>("to_map"_hs)
    .func<&assign<CTransform>, entt::as_ref_t>("assign"_hs);

  entt::meta<CLight>()
    .type("light"_hs)
    .ctor<&cast_clight, entt::as_ref_t>()
    .data<&CLight::_type>("type"_hs)
    .data<&CLight::color>("color"_hs)
    .data<&CLight::intensity>("intensity"_hs)
    .data<&CLight::range>("range"_hs)
    .data<&CLight::inner_cone_angle>("inner_cone_angle"_hs)
    .data<&CLight::outer_cone_angle>("outer_cone_angle"_hs)
    .func<&CLight::print>("print"_hs)
    .func<&CLight::to_map>("to_map"_hs)
    .func<&on_component_added<CLight>, entt::as_ref_t>("on_component_added"_hs)
    .func<&assign<CLight>, entt::as_ref_t>("assign"_hs);

  entt::meta<CSkybox>()
    .type("skybox"_hs)
    .ctor<&cast_cskybox, entt::as_ref_t>()
    .data<&CSkybox::enable_fog>("enable_fog"_hs)
    .data<&CSkybox::fog_color>("fog_color"_hs)
    .data<&CSkybox::fog_density>("fog_density"_hs)
    .data<&CSkybox::fog_gradient>("fog_gradient"_hs)
    .data<&CSkybox::rotation_speed>("rotation_speed"_hs)
    .func<&CSkybox::print>("print"_hs)
    .func<&CSkybox::to_map>("to_map"_hs)
    .func<&assign<CSkybox>, entt::as_ref_t>("assign"_hs);

  entt::meta<CTime>()
    .type("time"_hs)
    .ctor<&cast_ctime, entt::as_ref_t>()
    .data<&CTime::seconds>("seconds"_hs)
    .data<&CTime::current_hour>("current_hour"_hs)
    .data<&CTime::current_minute>("current_minute"_hs)
    .data<&CTime::current_second>("current_second"_hs)
    .data<&CTime::day_length>("day_length"_hs)
    .data<&CTime::starting_time>("starting_time"_hs)
    .data<&CTime::night_start>("night_start"_hs)
    .data<&CTime::day_transition_start>("day_transition_start"_hs)
    .data<&CTime::day_start>("day_start"_hs)
    .data<&CTime::night_transition_start>("night_transition_start"_hs)
    .data<&CTime::acceleration>("acceleration"_hs)
    .data<&CTime::fps>("fps"_hs)
    .func<&CTime::print>("print"_hs)
    .func<&CTime::to_map>("to_map"_hs)
    .func<&on_component_added<CTime>, entt::as_ref_t>("on_component_added"_hs)
    .func<&assign<CTime>, entt::as_ref_t>("assign"_hs);
}
}