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
#include "cMaterial.h"
#include "cTexture.h"
#include "cTextureAtlas.h"

using namespace entt::literals;

namespace nl {

struct CMesh {
    std::vector<std::shared_ptr<Texture>> textures;
    std::shared_ptr<VAO> vao;
    std::vector<Vertex> vertices; // TODO: delete this
    std::shared_ptr<VBO> vbo;
    std::vector<uint32_t> indices;
    std::string vertex_type;

    CMesh() = default;
    explicit CMesh(std::vector<Vertex>&& v, std::vector<uint32_t>&& i,
                   std::vector<std::shared_ptr<Texture>>&& t, std::string&& vt)
      : vertices(std::move(v)), indices(std::move(i)), textures(std::move(t)),
        vertex_type(std::move(vt)) {}

    void setup_mesh() {
      vao = VAO::create();
      if (vertex_type == "camera") {
        vao->attach_vertex(VBO::create(vertices), VAO::VertexType::vertex);
      } else if (vertex_type == "shape") { // TODO this is not used
        vao->attach_vertex(VBO::create(vertices),
                           VAO::VertexType::shape_vertex);
      } else if (vertex_type == "terrain") { // TODO maybe a better way to do
                                             // this using vertices?
        vao->attach_vertex(std::move(vbo), VAO::VertexType::terrain_vertex);
      } else {
        ENGINE_ASSERT(false, "unknown vertex type {}", vertex_type);
      }
      vao->set_index(IBO::create(indices));
    }

    void update_mesh() {
      if (vertex_type == "camera") {
        vao->update_vertex(VBO::create(vertices), 0, VAO::VertexType::vertex);
      } else if (vertex_type == "shape") {
        vao->update_vertex(VBO::create(vertices), 0,
                           VAO::VertexType::shape_vertex);
      } else if (vertex_type == "terrain") {
        vao->update_vertex(std::move(vbo), 0, VAO::VertexType::terrain_vertex);
      } else {
        ENGINE_ASSERT(false, "unknown vertex type {}", vertex_type);
      }
    }

    const std::shared_ptr<VAO>& get_vao() {
      if (not vao) {
        setup_mesh();
      }
      return vao;
    }

    // TODO move this method to a system?
    void bind_textures(ShaderProgram& sp,
                       CTexture* cTexture, CTextureAtlas* cTextureAtlas,
                       CTexture* cSkyboxTexture, CMaterial* cMaterial) {
      sp.reset_active_uniforms();
      sp.use();
      sp.set_float("enable_fog",
                    static_cast<float>(entt::monostate<"enable_fog"_hs>{}));
      sp.set_float("fog_density",
                    static_cast<float>(entt::monostate<"fog_density"_hs>{}));
      sp.set_float("fog_gradient",
                    static_cast<float>(entt::monostate<"fog_gradient"_hs>{}));
      sp.set_vec3("fog_color",
                   static_cast<glm::vec3>(entt::monostate<"fog_color"_hs>{}));

      if (cTexture) {
        uint32_t i = 1;
        for (auto& texture : cTexture->textures) {
          sp.set_int(texture->get_type().data() + std::to_string(i), i);
          texture->bind_slot(i);
          ++i;
        }
        if (cTexture->draw_mode == CTexture::DrawMode::color or
            cTexture->draw_mode == CTexture::DrawMode::texture_blend_color or
            cTexture->draw_mode ==
              CTexture::DrawMode::texture_atlas_blend_color) {
          sp.set_float("use_color", 1.f);
          sp.set_vec4("color", cTexture->color);
        }
        if (cTexture->enable_lighting) {
          sp.set_float("enable_lighting", 1.f);
          sp.set_vec3(
            "light_position",
            static_cast<glm::vec3>(entt::monostate<"light_position"_hs>{}));
          sp.set_vec3("light_color", static_cast<glm::vec3>(
                                        entt::monostate<"light_color"_hs>{}));
        }
        if (cTexture->draw_mode == CTexture::DrawMode::texture_atlas or
            cTexture->draw_mode == CTexture::DrawMode::texture_atlas_blend or
            cTexture->draw_mode ==
              CTexture::DrawMode::texture_atlas_blend_color) {
          if (sp.get_name() == "camera" or
              sp.get_name() ==
                "shape") { // terrain shader get texture atlas data from vertex
            sp.set_float("use_texture_atlas", 1.f);
            uint32_t index = cTextureAtlas->index;
            uint32_t rows = cTextureAtlas->rows;
            sp.set_float("rows", rows);
            uint32_t col = index % rows;
            float coll = static_cast<float>(col) / rows;
            uint32_t row = index / rows;
            float roww = static_cast<float>(row) / rows;
            sp.set_vec2("offset", glm::vec2(coll, roww));
          }
        }
        if (cTexture->draw_mode == CTexture::DrawMode::texture_blend or
            cTexture->draw_mode == CTexture::DrawMode::texture_atlas_blend or
            cTexture->draw_mode == CTexture::DrawMode::texture_blend_color or
            cTexture->draw_mode ==
              CTexture::DrawMode::texture_atlas_blend_color) {
          sp.set_float("use_blending", 1.f);
          sp.set_float("blend_factor", cTexture->blend_factor);
        }
        if (static_cast<float>(entt::monostate<"use_sky_blending"_hs>{}) ==
              1.f and
            sp.get_name() == "camera") {
          sp.set_float(
            "use_sky_blending",
            static_cast<float>(entt::monostate<"use_sky_blending"_hs>{}));
          sp.set_float(
            "skyblend_factor",
            static_cast<float>(entt::monostate<"skyblend_factor"_hs>{}));
          int ti = 10;
          for (auto& t : cSkyboxTexture->textures) {
            sp.set_int(t->get_type().data() + std::string("_sky_") +
                          std::to_string(ti),
                        ti);
            t->bind_slot(ti);
            ti++;
          }
          if (cTexture->enable_reflection) {
            sp.set_float("enable_reflection", 1.f);
            sp.set_float("reflectivity", cTexture->reflectivity);
          }
          if (cTexture->enable_refraction) {
            sp.set_float("enable_refraction", 1.f);
            sp.set_float("refractive_index", cTexture->refractive_index);
          }
        }
        if (cMaterial) {
          sp.set_vec3("ambient", cMaterial->ambient);
          sp.set_vec3("diffuse", cMaterial->diffuse);
          sp.set_vec3("specular", cMaterial->specular);
          sp.set_float("shininess", cMaterial->shininess);
        }
      } else { // models
        uint32_t diffuse_n = 1;
        uint32_t specular_n = 1;
        uint32_t normal_n = 1;
        uint32_t height_n = 1;
        uint32_t i = 1;
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
        if (textures.size() == 0) {
          ENGINE_ASSERT(cMaterial, "no material found for entity");
          sp.set_float("no_texture", 1.f);
          sp.set_vec3("material_color", cMaterial->diffuse);
        } else {
          if (diffuse_n == 1) {
            sp.set_float("use_normal", 1.f);
          }
        }
      }
      sp.unuse();
    }

    void unbind_textures(CTexture* cTexture) {
      auto& texturesToUnbind = (cTexture) ? cTexture->textures : textures;
      for (auto& texture : texturesToUnbind) {
        texture->unbind_slot();
      }
    }

    void print() const {
      std::string texturePaths;
      for (const auto& texture : textures) {
        texturePaths += std::format("\n\t\t\ttexture: {}", texture->get_path());
      }
      ENGINE_BACKTRACE("\t\tvertices: {0}\n\t\tindices: {1}{2}",
                       vertices.size(), indices.size(), texturePaths);
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

    std::string get_texture_info(uint32_t index) const {
      return map_to_json(textures.at(index)->to_map());
    }
};
}