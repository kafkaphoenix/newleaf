#pragma once

#include <entt/entt.hpp>
#include <format>
#include <glm/glm.hpp>
#include <map>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include "../../assets/texture.h"
#include "../../core/log_manager.h"
#include "../../render/buffer.h"
#include "../../render/shader_program.h"
#include "../../render/vao.h"
#include "../../utils/map_json_serializer.h"
#include "../../utils/numeric_comparator.h"
#include "../world/cSkybox.h"
#include "cMaterial.h"
#include "cTexture.h"
#include "cTextureAtlas.h"

using namespace entt::literals;

namespace nl::components {

struct CMesh {
    std::vector<std::shared_ptr<assets::Texture>> textures;
    std::shared_ptr<VAO> vao;
    std::vector<Vertex> vertices; // TODO: remove this
    std::shared_ptr<VBO> vbo;
    std::vector<uint32_t> indices;
    std::string vertex_type;

    CMesh() = default;
    explicit CMesh(std::vector<Vertex>&& v, std::vector<uint32_t>&& i,
                   std::vector<std::shared_ptr<assets::Texture>>&& t,
                   std::string&& vt)
      : vertices(std::move(v)), indices(std::move(i)), textures(std::move(t)),
        vertex_type(std::move(vt)) {}

    void setup_mesh() {
      vao = VAO::Create();
      if (vertex_type == "camera") {
        vao->attach_vertex(VBO::Create(vertices), VAO::VertexType::VERTEX);
      } else if (vertex_type == "shape") { // TODO this is not used
        vao->attach_vertex(VBO::Create(vertices),
                           VAO::VertexType::SHAPE_VERTEX);
      } else if (vertex_type == "terrain") { // TODO maybe a better way to do
                                             // this using vertices?
        vao->attach_vertex(std::move(vbo), VAO::VertexType::TERRAIN_VERTEX);
      } else {
        ENGINE_ASSERT(false, "Unknown vertex type {}", vertex_type);
      }
      vao->set_index(IBO::Create(indices));
    }

    void update_mesh() {
      if (vertex_type == "camera") {
        vao->update_vertex(VBO::Create(vertices), 0, VAO::VertexType::VERTEX);
      } else if (vertex_type == "shape") {
        vao->update_vertex(VBO::Create(vertices), 0,
                           VAO::VertexType::SHAPE_VERTEX);
      } else if (vertex_type == "terrain") {
        vao->update_vertex(std::move(vbo), 0, VAO::VertexType::TERRAIN_VERTEX);
      } else {
        ENGINE_ASSERT(false, "Unknown vertex type {}", vertex_type);
      }
    }

    const std::shared_ptr<VAO>& get_vao() {
      if (not vao) {
        setup_mesh();
      }
      return vao;
    }

    // TODO move this method to a system?
    void bind_textures(const std::unique_ptr<ShaderProgram>& sp,
                       CTexture* cTexture, CTextureAtlas* cTextureAtlas,
                       CTexture* cSkyboxTexture, CMaterial* cMaterial) {
      sp->reset_active_uniforms();
      sp->use();
      sp->set_float("enable_fog",
                    static_cast<float>(entt::monostate<"enable_fog"_hs>{}));
      sp->set_float("fog_density",
                    static_cast<float>(entt::monostate<"fog_density"_hs>{}));
      sp->set_float("fog_gradient",
                    static_cast<float>(entt::monostate<"fog_gradient"_hs>{}));
      sp->set_vec3("fog_color",
                   static_cast<glm::vec3>(entt::monostate<"fog_color"_hs>{}));

      if (cTexture) {
        uint32_t i = 1;
        for (auto& texture : cTexture->textures) {
          sp->set_int(texture->get_type().data() + std::to_string(i), i);
          texture->bind_slot(i);
          ++i;
        }
        if (cTexture->draw_mode == CTexture::DrawMode::COLOR or
            cTexture->draw_mode == CTexture::DrawMode::TEXTURE_BLEND_COLOR or
            cTexture->draw_mode ==
              CTexture::DrawMode::TEXTURE_ATLAS_BLEND_COLOR) {
          sp->set_float("useColor", 1.f);
          sp->set_vec4("color", cTexture->color);
        }
        if (cTexture->enable_lighting) {
          sp->set_float("enable_lighting", 1.f);
          sp->set_vec3(
            "lightPosition",
            static_cast<glm::vec3>(entt::monostate<"lightPosition"_hs>{}));
          sp->set_vec3("lightColor", static_cast<glm::vec3>(
                                       entt::monostate<"lightColor"_hs>{}));
        }
        if (cTexture->draw_mode == CTexture::DrawMode::TEXTURE_ATLAS or
            cTexture->draw_mode == CTexture::DrawMode::TEXTURE_ATLAS_BLEND or
            cTexture->draw_mode ==
              CTexture::DrawMode::TEXTURE_ATLAS_BLEND_COLOR) {
          if (sp->get_name() == "camera" or
              sp->get_name() ==
                "shape") { // terrain shader get texture atlas data from vertex
            sp->set_float("useTextureAtlas", 1.f);
            uint32_t index = cTextureAtlas->index;
            uint32_t rows = cTextureAtlas->rows;
            sp->set_float("numRows", rows);
            uint32_t col = index % rows;
            float coll = static_cast<float>(col) / rows;
            uint32_t row = index / rows;
            float roww = static_cast<float>(row) / rows;
            sp->set_vec2("offset", glm::vec2(coll, roww));
          }
        }
        if (cTexture->draw_mode == CTexture::DrawMode::TEXTURES_BLEND or
            cTexture->draw_mode == CTexture::DrawMode::TEXTURE_ATLAS_BLEND or
            cTexture->draw_mode == CTexture::DrawMode::TEXTURE_BLEND_COLOR or
            cTexture->draw_mode ==
              CTexture::DrawMode::TEXTURE_ATLAS_BLEND_COLOR) {
          sp->set_float("useBlending", 1.f);
          sp->set_float("blend_factor", cTexture->blend_factor);
        }
        if (static_cast<float>(entt::monostate<"useSkyBlending"_hs>{}) ==
              1.f and
            sp->get_name() == "camera") {
          sp->set_float(
            "useSkyBlending",
            static_cast<float>(entt::monostate<"useSkyBlending"_hs>{}));
          sp->set_float(
            "skyblend_factor",
            static_cast<float>(entt::monostate<"skyblend_factor"_hs>{}));
          int ti = 10;
          for (auto& t : cSkyboxTexture->textures) {
            sp->set_int(t->get_type().data() + std::string("Sky") +
                          std::to_string(ti),
                        ti);
            t->bind_slot(ti);
            ti++;
          }
          if (cTexture->enable_reflection) {
            sp->set_float("enable_reflection", 1.f);
            sp->set_float("reflectivity", cTexture->reflectivity);
          }
          if (cTexture->enable_refraction) {
            sp->set_float("enable_refraction", 1.f);
            sp->set_float("refractive_index", cTexture->refractive_index);
          }
        }
        if (cMaterial) {
          sp->set_vec3("ambient", cMaterial->ambient);
          sp->set_vec3("diffuse", cMaterial->diffuse);
          sp->set_vec3("specular", cMaterial->specular);
          sp->set_float("shininess", cMaterial->shininess);
        }
      } else {
        uint32_t diffuseN = 1;
        uint32_t specularN = 1;
        uint32_t normalN = 1;
        uint32_t heightN = 1;
        uint32_t i = 1;
        for (auto& texture : textures) {
          std::string number;
          std::string_view type = texture->get_type();
          if (type == "textureDiffuse") {
            number = std::to_string(diffuseN++);
          } else if (type == "textureSpecular") {
            number = std::to_string(specularN++);
          } else if (type == "textureNormal") {
            number = std::to_string(normalN++);
          } else if (type == "textureHeight") {
            number = std::to_string(heightN++);
          } else {
            ENGINE_ASSERT(false, "Unknown texture type {}", type);
          }
          sp->set_int(type.data() + number, i);
          texture->bind_slot(i);
          ++i;
        }
        if (textures.size() == 0) {
          ENGINE_ASSERT(cMaterial, "No material found for entity");
          sp->set_float("noTexture", 1.f);
          sp->set_vec3("materialColor", cMaterial->diffuse);
        } else {
          if (diffuseN == 1) {
            sp->set_float("useNormal", 1.f);
          }
        }
      }
      sp->unuse();
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
        info["texture " + std::to_string(i)] = get_texture_info(i);
      }
      info["vao 0"] = vao ? get_vao_info() : "undefined";
      info["vertex_type"] = vertex_type;

      return info;
    }

    std::string get_vao_info() const { return map_to_json(vao->to_map()); }

    std::string get_texture_info(uint32_t index) const {
      return map_to_json(textures.at(index)->to_map());
    }
};
}