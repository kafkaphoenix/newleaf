#pragma once

#include <format>
#include <map>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include <entt/entt.hpp>
#include <glm/glm.hpp>

#include "../../assets/texture.h"
#include "../../graphics/buffer.h"
#include "../../graphics/shader_program.h"
#include "../../graphics/vao.h"
#include "../../logging/log_manager.h"
#include "../../utils/assert.h"
#include "../../utils/map_json_serializer.h"
#include "../../utils/numeric_comparator.h"
#include "../world/cSkybox.h"
#include "cBlendColor.h"
#include "cBlendTexture.h"
#include "cColor.h"
#include "cMaterial.h"
#include "cReflection.h"
#include "cTexture.h"
#include "cTextureAtlas.h"

using namespace entt::literals;

namespace nl {

struct CMesh {
    std::vector<std::shared_ptr<Texture>> textures;
    std::shared_ptr<VAO> vao;
    std::vector<ModelVertex> vertices; // TODO: delete this
    std::shared_ptr<VBO> vbo;
    std::vector<uint32_t> indices;
    std::string vertex_type;

    CMesh() = default;
    explicit CMesh(std::vector<ModelVertex>&& v, std::vector<uint32_t>&& i, std::vector<std::shared_ptr<Texture>>&& t,
                   std::string&& vt)
      : vertices(std::move(v)), indices(std::move(i)), textures(std::move(t)), vertex_type(std::move(vt)) {}

    void setup_mesh() {
      vao = VAO::create();
      if (vertex_type == "model") {
        vao->attach_vertex(VBO::CreateModel(vertices), VAO::VertexType::Model);
      } else if (vertex_type ==
                 "shape") { // TODO this is not used and use wrong method, shape factory  use create shape
        vao->attach_vertex(VBO::CreateModel(vertices), VAO::VertexType::Shape);
      } else if (vertex_type == "terrain") { // TODO maybe a better way to do
                                             // this using vertices? terrain returns vbo from other side
        vao->attach_vertex(std::move(vbo), VAO::VertexType::Terrain);
      } else {
        ENGINE_ASSERT(false, "unknown vertex type {}", vertex_type);
      }
      vao->set_index(IBO::create(indices));
    }

    const std::shared_ptr<VAO>& get_vao() {
      if (not vao) {
        setup_mesh();
      }
      return vao;
    }

    // TODO rethink with uniform buffer object in system
    void configure_fog(ShaderProgram& sp) {
      sp.set_float("fog_enabled", static_cast<float>(entt::monostate<"fog_enabled"_hs>{}));
      sp.set_vec4("fog_color", static_cast<glm::vec4>(entt::monostate<"fog_color"_hs>{}));
      sp.set_float("fog_density", static_cast<float>(entt::monostate<"fog_density"_hs>{}));
      sp.set_float("fog_gradient", static_cast<float>(entt::monostate<"fog_gradient"_hs>{}));
      sp.set_float("fog_lower_limit", static_cast<float>(entt::monostate<"fog_lower_limit"_hs>{}));
      sp.set_float("fog_upper_limit", static_cast<float>(entt::monostate<"fog_upper_limit"_hs>{}));
    }

    // TODO rethink with uniform buffer object in system
    void configure_light(ShaderProgram& sp) {
      sp.set_float("light_enabled", static_cast<float>(entt::monostate<"light_enabled"_hs>{}));
      sp.set_vec3("light_color", static_cast<glm::vec3>(entt::monostate<"light_color"_hs>{}));
      sp.set_vec3("light_position", static_cast<glm::vec3>(entt::monostate<"light_position"_hs>{}));
      sp.set_float("light_intensity", static_cast<float>(entt::monostate<"light_intensity"_hs>{}));
      sp.set_float("light_range", static_cast<float>(entt::monostate<"light_range"_hs>{}));
      sp.set_float("light_inner_cone_angle", static_cast<float>(entt::monostate<"light_inner_cone_angle"_hs>{}));
      sp.set_float("light_outer_cone_angle", static_cast<float>(entt::monostate<"light_outer_cone_angle"_hs>{}));
    }

    void configure_reflection(ShaderProgram& sp, CReflection* cReflection) {
      if (cReflection) {
        sp.set_float("reflection_enabled", cReflection->enabled ? 1.f : 0.f);
        sp.set_float("reflectivity", cReflection->reflectivity);
        sp.set_float("refractivity", cReflection->refractivity);
      }
    }

    void configure_material(ShaderProgram& sp, CMaterial* cMaterial) {
      if (cMaterial) {
        sp.set_vec3("ambient", cMaterial->ambient);
        sp.set_vec3("diffuse", cMaterial->diffuse);
        sp.set_vec3("specular", cMaterial->specular);
        sp.set_float("shininess", cMaterial->shininess);
      }
    }

    // TODO move to system and rethink with uniform buffer object in system
    void configure_texture_atlas(ShaderProgram& sp, CTextureAtlas* cTextureAtlas) {
      // TODO terrain shader not using this logic at all (get from terrain vertex directly)
      if (cTextureAtlas) {
        if (sp.get_name() == "shape") {
          sp.set_float("texture_atlas_enabled", 1.f);
          uint32_t index = cTextureAtlas->index;
          uint32_t rows = cTextureAtlas->rows;
          sp.set_float("texture_atlas_rows", rows);
          uint32_t col = index % rows;
          float coll = static_cast<float>(col) / rows;
          uint32_t row = index / rows;
          float roww = static_cast<float>(row) / rows;
          sp.set_vec2("texture_atlas_offset", glm::vec2(coll, roww));
        }
      } else {
        sp.set_float("texture_atlas_enabled", 0.f);
      }
    }

    void configure_skybox(ShaderProgram& sp, CTexture* cSkyboxTexture, CBlendTexture* cSkyboxBlend) {
      // TODO better reflection than sending the skybox texture and the blend
      // that's why we send them to all shaders
      // TODO use uniform buffer object for this
      if (cSkyboxTexture) {
        // 10 and 11 are reserved for skybox
        sp.set_int("skybox_texture", 10);
        // we only have one texture for skybox cubemap
        cSkyboxTexture->textures[0]->bind_slot(10);
        if (cSkyboxBlend) {
          sp.set_float("blend_skybox_enabled", 1.f);
          sp.set_int("blend_skybox_texture", 11);
          cSkyboxBlend->texture->bind_slot(11);
          sp.set_float("blend_skybox_factor", cSkyboxBlend->blend_factor);
        } else {
          sp.set_float("blend_skybox_enabled", 0.f);
        }
      }
    }

    void configure_color(ShaderProgram& sp, CColor* cColor) {
      if (cColor) {
        sp.set_float("color_enabled", 1.f);
        sp.set_vec4("color", cColor->color);
      } else {
        sp.set_float("color_enabled", 0.f);
      }
    }

    void configure_blend(ShaderProgram& sp, CBlendTexture* cBlendTexture, CBlendColor* cBlendColor) {
      if (cBlendTexture) {
        sp.set_float("blend_texture_enabled", 1.f);
        sp.set_float("blend_texture_factor", cBlendTexture->blend_factor);
        sp.set_int("blend_texture", 9); // slot 9 reserved for blend texture
        cBlendTexture->texture->bind_slot(9);
      } else {
        sp.set_float("blend_texture_enabled", 0.f);
      }
      if (cBlendColor) {
        sp.set_float("blend_color_enabled", 1.f);
        sp.set_float("blend_color_factor", cBlendColor->blend_factor);
        sp.set_vec4("blend_color", cBlendColor->color);
      } else {
        sp.set_float("blend_color_enabled", 0.f);
      }
    }

    void configure_model_texture(ShaderProgram& sp, CMaterial* cMaterial) {
      uint32_t diffuse_n = 1;
      uint32_t specular_n = 1;
      uint32_t normal_n = 1;
      uint32_t height_n = 1;
      uint32_t i = 1;
      // TODO improve code for several textures and to select normal/material
      // with arrays
      if (textures.size() == 0) {
        sp.set_float("texture_enabled", 0.f); // will use material only
        ENGINE_ASSERT(cMaterial, "no texture or material found for model");
        return;
      }
      sp.set_float("texture_enabled", 1.f);
      for (auto& texture : textures) {
        std::string number;
        std::string_view type = texture->get_type();
        if (type == "texture_diffuse") {
          number = std::to_string(diffuse_n++);
        } else if (type == "texture_specular") {
          number = std::to_string(specular_n++);
        } else if (type == "texture_normal") {
          number = std::to_string(normal_n++);
        } else if (type == "texture_height") {
          number = std::to_string(height_n++);
        } else {
          ENGINE_ASSERT(false, "unknown texture type {}", type);
        }
        sp.set_int(type.data() + std::string("_") + number, i);
        texture->bind_slot(i);
        ++i;
      }
      // TODO rethink how to use normal and other together
      sp.set_float("normal_enabled", normal_n > 1 ? 1.f : 0.f);
    }

    // TODO remove this and rethink in systems with uniform buffer objects
    void bind_textures(ShaderProgram& sp, CTexture* cTexture, CBlendTexture* cBlendTexture,
                       CTextureAtlas* cTextureAtlas, CColor* cColor, CBlendColor* cBlendColor, CMaterial* cMaterial,
                       CReflection* cReflection, CSkybox* cSkybox, CTexture* cSkyboxTexture,
                       CBlendTexture* cSkyboxBlend) {
      sp.reset_active_uniforms();
      sp.use();
      configure_fog(sp);
      configure_light(sp);
      configure_reflection(sp, cReflection);
      configure_material(sp, cMaterial);
      configure_texture_atlas(sp, cTextureAtlas);
      configure_skybox(sp, cSkyboxTexture, cSkyboxBlend);
      configure_color(sp, cColor);
      // we skip sky as we do it in configure skybox TODO rethink
      if (not cSkybox) {
        configure_blend(sp, cBlendTexture, cBlendColor);
      }

      if (cTexture) {
        uint32_t i = 1;
        for (auto& texture : cTexture->textures) {
          sp.set_int(texture->get_type().data() + std::string("_") + std::to_string(i), i);
          texture->bind_slot(i);
          ++i;
        }
      } else if (not cTexture) { // model texture
        // TODO add model textures to CTexture and remove this and use only one shader maybe
        configure_model_texture(sp, cMaterial);
      }
      sp.unuse();
    }

    void unbind_textures(CTexture* cTexture, CTextureAtlas* cTextureAtlas, CBlendTexture* cBlendTexture) {
      auto& unbind_textures = textures; // model textures
      if (cTexture) {
        unbind_textures = cTexture->textures;
      } else if (cTextureAtlas) {
        unbind_textures = {cTextureAtlas->texture};
      } else if (cBlendTexture) {
        unbind_textures = {cBlendTexture->texture};
      }
      for (auto& texture : unbind_textures) {
        texture->unbind_slot();
      }
    }

    void print() const {
      std::string paths;
      for (const auto& texture : textures) {
        paths += std::format("\n\t\t\ttexture: {}", texture->get_path());
      }
      ENGINE_BACKTRACE("\t\tvertices: {0}\n\t\tindices: {1}{2}", vertices.size(), indices.size(), paths);
    }

    std::map<std::string, std::string, NumericComparator> to_map() const {
      std::map<std::string, std::string, NumericComparator> info;
      for (uint32_t i = 0; i < textures.size(); ++i) {
        info["texture_" + std::to_string(i)] = get_texture_info(i);
      }
      info["vao_0"] = vao ? get_vao_info() : "undefined";
      info["vertex_type"] = vertex_type;

      return info;
    }

    std::string get_vao_info() const { return map_to_json(vao->to_map()); }

    std::string get_texture_info(uint32_t index) const { return map_to_json(textures.at(index)->to_map()); }
};
}