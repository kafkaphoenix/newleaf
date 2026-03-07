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
struct CMesh;
struct CShader;
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
struct CFog;
struct CReflection;
struct CTransparent;
struct CBlendColor;
struct CBlendTexture;
struct CColor;

inline CActiveCamera& cast_cactive_camera(void* other) { return *static_cast<CActiveCamera*>(other); }

inline CCamera& cast_ccamera(void* other) { return *static_cast<CCamera*>(other); }

inline CDistanceFromCamera& cast_cdistance_from_camera(void* other) {
  return *static_cast<CDistanceFromCamera*>(other);
}

inline CName& cast_cname(void* other) { return *static_cast<CName*>(other); }

inline CTag& cast_ctag(void* other) { return *static_cast<CTag*>(other); }

inline CUUID& cast_cuuid(void* other) { return *static_cast<CUUID*>(other); }

inline CBody& cast_cbody(void* other) { return *static_cast<CBody*>(other); }

inline CFBO& cast_cfbo(void* other) { return *static_cast<CFBO*>(other); }

inline CMesh& cast_cmesh(void* other) { return *static_cast<CMesh*>(other); }

inline CShader& cast_cshader(void* other) { return *static_cast<CShader*>(other); }

inline CShape& cast_cshape(void* other) { return *static_cast<CShape*>(other); }

inline CTexture& cast_ctexture(void* other) { return *static_cast<CTexture*>(other); }

inline CTextureAtlas& cast_ctexture_atlas(void* other) { return *static_cast<CTextureAtlas*>(other); }

inline CActiveInput& cast_cactive_input(void* other) { return *static_cast<CActiveInput*>(other); }

inline CInput& cast_cinput(void* other) { return *static_cast<CInput*>(other); }

inline CCollider& cast_ccollider(void* other) { return *static_cast<CCollider*>(other); }

inline CGravity& cast_cgravity(void* other) { return *static_cast<CGravity*>(other); }

inline CRigidBody& cast_crigid_body(void* other) { return *static_cast<CRigidBody*>(other); }

inline CTransform& cast_ctransform(void* other) { return *static_cast<CTransform*>(other); }

inline CLight& cast_clight(void* other) { return *static_cast<CLight*>(other); }

inline CSkybox& cast_cskybox(void* other) { return *static_cast<CSkybox*>(other); }

inline CFog& cast_cfog(void* other) { return *static_cast<CFog*>(other); }

inline CTime& cast_ctime(void* other) { return *static_cast<CTime*>(other); }

inline CReflection& cast_creflection(void* other) { return *static_cast<CReflection*>(other); }

inline CTransparent& cast_ctransparent(void* other) { return *static_cast<CTransparent*>(other); }

inline CBlendColor& cast_cblend_color(void* other) { return *static_cast<CBlendColor*>(other); }

inline CBlendTexture& cast_cblend_texture(void* other) { return *static_cast<CBlendTexture*>(other); }

inline CColor& cast_ccolor(void* other) { return *static_cast<CColor*>(other); }

void register_components();
}