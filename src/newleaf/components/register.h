#pragma once

#include <any>

namespace nl {

struct CActiveCamera;
struct CCamera;
struct CDistanceFromCamera;
struct CName;
struct CTag;
struct CUUID;
struct CModel;
struct CFBO;
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

inline CActiveCamera& cast_cactive_camera(std::any other) { return *std::any_cast<CActiveCamera*>(other); }

inline CCamera& cast_ccamera(std::any other) { return *std::any_cast<CCamera*>(other); }

inline CDistanceFromCamera& cast_cdistance_from_camera(std::any other) {
  return *std::any_cast<CDistanceFromCamera*>(other);
}

inline CName& cast_cname(std::any other) { return *std::any_cast<CName*>(other); }

inline CTag& cast_ctag(std::any other) { return *std::any_cast<CTag*>(other); }

inline CUUID& cast_cuuid(std::any other) { return *std::any_cast<CUUID*>(other); }

inline CModel& cast_CModel(std::any other) { return *std::any_cast<CModel*>(other); }

inline CFBO& cast_cfbo(std::any other) { return *std::any_cast<CFBO*>(other); }

inline CShader& cast_cshader(std::any other) { return *std::any_cast<CShader*>(other); }

inline CShape& cast_cshape(std::any other) { return *std::any_cast<CShape*>(other); }

inline CTexture& cast_ctexture(std::any other) { return *std::any_cast<CTexture*>(other); }

inline CTextureAtlas& cast_ctexture_atlas(std::any other) { return *std::any_cast<CTextureAtlas*>(other); }

inline CActiveInput& cast_cactive_input(std::any other) { return *std::any_cast<CActiveInput*>(other); }

inline CInput& cast_cinput(std::any other) { return *std::any_cast<CInput*>(other); }

inline CCollider& cast_ccollider(std::any other) { return *std::any_cast<CCollider*>(other); }

inline CGravity& cast_cgravity(std::any other) { return *std::any_cast<CGravity*>(other); }

inline CRigidBody& cast_crigid_body(std::any other) { return *std::any_cast<CRigidBody*>(other); }

inline CTransform& cast_ctransform(std::any other) { return *std::any_cast<CTransform*>(other); }

inline CLight& cast_clight(std::any other) { return *std::any_cast<CLight*>(other); }

inline CSkybox& cast_cskybox(std::any other) { return *std::any_cast<CSkybox*>(other); }

inline CFog& cast_cfog(std::any other) { return *std::any_cast<CFog*>(other); }

inline CTime& cast_ctime(std::any other) { return *std::any_cast<CTime*>(other); }

inline CReflection& cast_creflection(std::any other) { return *std::any_cast<CReflection*>(other); }

inline CTransparent& cast_ctransparent(std::any other) { return *std::any_cast<CTransparent*>(other); }

inline CBlendColor& cast_cblend_color(std::any other) { return *std::any_cast<CBlendColor*>(other); }

inline CBlendTexture& cast_cblend_texture(std::any other) { return *std::any_cast<CBlendTexture*>(other); }

inline CColor& cast_ccolor(std::any other) { return *std::any_cast<CColor*>(other); }

void register_components();
}