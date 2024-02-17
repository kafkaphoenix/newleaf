#pragma once

#include <entt/entt.hpp>

#include "core/application.h"
#include "scene/components/core/cName.h"
#include "scene/components/core/cTag.h"
#include "scene/components/core/cUUID.h"
#include "scene/components/graphics/cMaterial.h"
#include "scene/components/graphics/cMesh.h"
#include "scene/components/graphics/cTexture.h"
#include "scene/components/graphics/cTextureAtlas.h"
#include "scene/components/world/cSkybox.h"
#include "scene/meta.h"

using namespace entt::literals;

namespace potatoengine {

CUUID& CastCUUID(void* other) { return *static_cast<CUUID*>(other); }

CName& CastCName(void* other) { return *static_cast<CName*>(other); }

CTag& CastCTag(void* other) { return *static_cast<CTag*>(other); }

CMaterial& CastCMaterial(void* other) {
  return *static_cast<CMaterial*>(other);
}

CTextureAtlas& CastCTextureAtlas(void* other) {
  return *static_cast<CTextureAtlas*>(other);
}

CTexture& CastCTexture(void* other) { return *static_cast<CTexture*>(other); }

CMesh& CastCMesh(void* other) { return *static_cast<CMesh*>(other); }

CSkybox& CastCSkybox(void* other) { return *static_cast<CSkybox*>(other); }

void RegisterComponents() {
  entt::meta<CUUID>()
    .type("uuid"_hs)
    .ctor<&CastCUUID, entt::as_ref_t>()
    .data<&CUUID::uuid>("uuid"_hs)
    .func<&CUUID::print>("print"_hs)
    .func<&CUUID::getInfo>("getInfo"_hs);

  entt::meta<CName>()
    .type("name"_hs)
    .ctor<&CastCName, entt::as_ref_t>()
    .data<&CName::name>("name"_hs)
    .func<&CName::print>("print"_hs)
    .func<&CName::getInfo>("getInfo"_hs)
    .func<&assign<CName, std::string>, entt::as_ref_t>("assign"_hs);

  entt::meta<CTag>()
    .type("tag"_hs)
    .ctor<&CastCTag, entt::as_ref_t>()
    .data<&CTag::tag>("tag"_hs)
    .func<&CTag::print>("print"_hs)
    .func<&CTag::getInfo>("getInfo"_hs)
    .func<&assign<CTag, std::string>, entt::as_ref_t>("assign"_hs);

  entt::meta<CMaterial>()
    .type("material"_hs)
    .ctor<&CastCMaterial, entt::as_ref_t>()
    .data<&CMaterial::ambient>("ambient"_hs)
    .data<&CMaterial::diffuse>("diffuse"_hs)
    .data<&CMaterial::specular>("specular"_hs)
    .data<&CMaterial::shininess>("shininess"_hs)
    .func<&CMaterial::print>("print"_hs)
    .func<&CMaterial::getInfo>("getInfo"_hs)
    .func<&assign<CMaterial>, entt::as_ref_t>("assign"_hs);

  entt::meta<CTextureAtlas>()
    .type("textureAtlas"_hs)
    .ctor<&CastCTextureAtlas, entt::as_ref_t>()
    .data<&CTextureAtlas::rows>("rows"_hs)
    .data<&CTextureAtlas::index>("index"_hs)
    .func<&CTextureAtlas::print>("print"_hs)
    .func<&CTextureAtlas::getInfo>("getInfo"_hs)
    .func<&assign<CTextureAtlas>, entt::as_ref_t>("assign"_hs);

  entt::meta<CTexture>()
    .type("texture"_hs)
    .ctor<&CastCTexture, entt::as_ref_t>()
    .data<&CTexture::filepaths>("filepaths"_hs)
    .data<&CTexture::textures>("textures"_hs)
    .data<&CTexture::color>("color"_hs)
    .data<&CTexture::blendFactor>("blendFactor"_hs)
    .data<&CTexture::reflectivity>("reflectivity"_hs)
    .data<&CTexture::refractiveIndex>("refractiveIndex"_hs)
    .data<&CTexture::hasTransparency>("hasTransparency"_hs)
    .data<&CTexture::useLighting>("useLighting"_hs)
    .data<&CTexture::useReflection>("useReflection"_hs)
    .data<&CTexture::useRefraction>("useRefraction"_hs)
    .data<&CTexture::_drawMode>("drawMode"_hs)
    .func<&CTexture::print>("print"_hs)
    .func<&CTexture::getInfo>("getInfo"_hs)
    .func<&onComponentAdded<CTexture>, entt::as_ref_t>("onComponentAdded"_hs)
    .func<&assign<CTexture>, entt::as_ref_t>("assign"_hs);

  entt::meta<CMesh>()
    .type("mesh"_hs)
    .ctor<&CastCMesh, entt::as_ref_t>()
    .data<&CMesh::vertices>("vertices"_hs)
    .data<&CMesh::indices>("indices"_hs)
    .data<&CMesh::textures>("textures"_hs)
    .data<&CMesh::vao>("vao"_hs)
    .data<&CMesh::vertexType>("vertexType"_hs)
    .func<&CMesh::print>("print"_hs)
    .func<&CMesh::getInfo>("getInfo"_hs)
    .func<&assign<CMesh>, entt::as_ref_t>("assign"_hs);

  entt::meta<CSkybox>()
    .type("skybox"_hs)
    .ctor<&CastCSkybox, entt::as_ref_t>()
    .data<&CSkybox::useFog>("useFog"_hs)
    .data<&CSkybox::fogColor>("fogColor"_hs)
    .data<&CSkybox::fogDensity>("fogDensity"_hs)
    .data<&CSkybox::fogGradient>("fogGradient"_hs)
    .data<&CSkybox::rotationSpeed>("rotationSpeed"_hs)
    .func<&CSkybox::print>("print"_hs)
    .func<&CSkybox::getInfo>("getInfo"_hs)
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