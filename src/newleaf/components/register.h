#pragma once

namespace nl {

struct CActiveCamera;
struct CCamera;
struct CDistanceFromCamera;
struct CName;
struct CTag;
struct CUUID;
struct CBody;
struct CFBO;
struct CMaterial;
struct CMesh;
struct CShaderProgram;
struct CShape;
struct CTexture;
struct CTextureAtlas;
struct CActiveInput;
struct CInput;
struct CCollider;
struct CGravity;
struct CRigidBody;
struct CTransform;
struct CLight;
struct CSkybox;
struct CTime;

inline CActiveCamera& cast_cactive_camera(void* other) {
  return *static_cast<CActiveCamera*>(other);
}

inline CCamera& cast_ccamera(void* other) {
  return *static_cast<CCamera*>(other);
}

inline CDistanceFromCamera& cast_cdistance_from_camera(void* other) {
  return *static_cast<CDistanceFromCamera*>(other);
}

inline CName& cast_cname(void* other) { return *static_cast<CName*>(other); }

inline CTag& cast_ctag(void* other) { return *static_cast<CTag*>(other); }

inline CUUID& cast_cuuid(void* other) { return *static_cast<CUUID*>(other); }

inline CBody& cast_cbody(void* other) { return *static_cast<CBody*>(other); }

inline CFBO& cast_cfbo(void* other) { return *static_cast<CFBO*>(other); }

inline CMaterial& cast_cmaterial(void* other) {
  return *static_cast<CMaterial*>(other);
}

inline CMesh& cast_cmesh(void* other) { return *static_cast<CMesh*>(other); }

inline CShaderProgram& cast_cshader_program(void* other) {
  return *static_cast<CShaderProgram*>(other);
}

inline CShape& cast_cshape(void* other) { return *static_cast<CShape*>(other); }

inline CTexture& cast_ctexture(void* other) {
  return *static_cast<CTexture*>(other);
}

inline CTextureAtlas& cast_ctexture_atlas(void* other) {
  return *static_cast<CTextureAtlas*>(other);
}

inline CActiveInput& cast_cactive_input(void* other) {
  return *static_cast<CActiveInput*>(other);
}

inline CInput& cast_cinput(void* other) { return *static_cast<CInput*>(other); }

inline CCollider& cast_ccollider(void* other) {
  return *static_cast<CCollider*>(other);
}

inline CGravity& cast_cgravity(void* other) {
  return *static_cast<CGravity*>(other);
}

inline CRigidBody& cast_crigid_body(void* other) {
  return *static_cast<CRigidBody*>(other);
}

inline CTransform& cast_ctransform(void* other) {
  return *static_cast<CTransform*>(other);
}

inline CLight& cast_clight(void* other) { return *static_cast<CLight*>(other); }

inline CSkybox& cast_cskybox(void* other) {
  return *static_cast<CSkybox*>(other);
}

inline CTime& cast_ctime(void* other) { return *static_cast<CTime*>(other); }

void register_components();
}