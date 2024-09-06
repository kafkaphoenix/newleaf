#pragma once

#include <entt/entt.hpp>

#include "../core/application.h"
#include "../core/log_manager.h"
#include "components/core/cName.h"
#include "components/core/cTag.h"
#include "components/core/cUUID.h"
#include "components/graphics/cMaterial.h"
#include "components/graphics/cMesh.h"
#include "components/graphics/cTexture.h"
#include "components/graphics/cTextureAtlas.h"
#include "components/world/cSkybox.h"
#include "meta.h"

#include <string_view>

using namespace entt::literals;

namespace nl {

CUUID& cast_cuuid(void* other) { return *static_cast<CUUID*>(other); }

CName& cast_cname(void* other) { return *static_cast<CName*>(other); }

CTag& cast_ctag(void* other) { return *static_cast<CTag*>(other); }

CMaterial& cast_cmaterial(void* other) {
  return *static_cast<CMaterial*>(other);
}

CTextureAtlas& cast_ctexture_atlas(void* other) {
  return *static_cast<CTextureAtlas*>(other);
}

CTexture& cast_ctexture(void* other) { return *static_cast<CTexture*>(other); }

CMesh& cast_cmesh(void* other) { return *static_cast<CMesh*>(other); }

CSkybox& cast_cskybox(void* other) { return *static_cast<CSkybox*>(other); }

void register_components() {
  entt::meta<CUUID>()
    .type("uuid"_hs)
    .ctor<&cast_cuuid, entt::as_ref_t>()
    .data<&CUUID::uuid>("uuid"_hs)
    .func<&CUUID::print>("print"_hs)
    .func<&CUUID::to_map>("to_map"_hs);

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

  entt::meta<CTextureAtlas>()
    .type("textureAtlas"_hs)
    .ctor<&cast_ctexture_atlas, entt::as_ref_t>()
    .data<&CTextureAtlas::rows>("rows"_hs)
    .data<&CTextureAtlas::index>("index"_hs)
    .func<&CTextureAtlas::print>("print"_hs)
    .func<&CTextureAtlas::to_map>("to_map"_hs)
    .func<&assign<CTextureAtlas>, entt::as_ref_t>("assign"_hs);

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
}

void PrintScene(entt::registry& registry) {
  auto entities = registry.view<CUUID>();
  entt::meta_type cType;
  entt::meta_any cData;
  entt::meta_func printFunc;
  std::string_view cName;
  if (entities.empty()) {
    ENGINE_BACKTRACE("===================Entities===================");
    ENGINE_BACKTRACE("No entities in scene");
    ENGINE_BACKTRACE("=============================================");
    return;
  }

  ENGINE_BACKTRACE("===================Entities===================");
  ENGINE_BACKTRACE("Entities in scene: {}", entities.size());
  for (const auto& e : entities) {
    ENGINE_BACKTRACE("Entity UUID: {}", entt::to_integral(e));
    for (const auto& [id, storage] : registry.storage()) {
      if (storage.contains(e)) {
        cType = entt::resolve(storage.type());
        cData = cType.construct(storage.value(e));
        printFunc = cType.func("print"_hs);
        if (printFunc) {
          cName = storage.type().name();
          cName = cName.substr(cName.find_last_of(':') + 1);
          ENGINE_BACKTRACE("\t{}", cName);
          printFunc.invoke(cData);
        } else {
          cName = storage.type().name();
          cName = cName.substr(cName.find_last_of(':') + 1);
          ENGINE_ERROR("{} has no print function", cName);
          ENGINE_BACKTRACE("\t{} has no print function", cName);
        }
      }
    }
  }
  ENGINE_BACKTRACE("=============================================");
}
}