#pragma once

#include <glm/glm.hpp>
#include <glm/vec4.hpp>
#include <map>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include "../../assets/asset.h"
#include "../../assets/asset_handle.h"
#include "../../assets/shader.h"
#include "../../assets/texture.h"
#include "../../utils/numeric_comparator.h"

namespace nl {

struct RenderState {
    bool blend = false;
    bool depth_write = true;
    bool cull = true;
};

struct MaterialTextures {
    AssetHandle<Texture> base_color;
    AssetHandle<Texture> metallic_roughness;
    AssetHandle<Texture> normal;
    AssetHandle<Texture> emissive;
    AssetHandle<Texture> occlusion;
};

struct MaterialParams {
    glm::vec4 base_color_factor{1.0f, 1.0f, 1.0f, 1.0f};
    float metallic_factor = 1.0f;
    float roughness_factor = 1.0f;
    glm::vec3 emissive_factor{0.0f, 0.0f, 0.0f};
    float alpha_cutoff = 0.5f;
};

class Material : public Asset {
  public:
    Material() = default;
    Material(std::string name, AssetHandle<Shader> shader, const MaterialTextures& textures,
             const MaterialParams& params, const RenderState& state);
    ~Material() override = default;

    const AssetHandle<Shader>& get_shader_handle() const { return m_shader; }
    const AssetHandle<Texture>& get_base_color_handle() const { return m_textures.base_color; }
    const MaterialTextures& get_textures() const { return m_textures; }
    const MaterialParams& get_params() const { return m_params; }
    const RenderState& get_state() const { return m_state; }
    std::string_view get_path() const { return m_path; }
    const std::map<std::string, std::string, NumericComparator>& to_map() override final;
    bool operator==(const Asset& other) const override final;

  private:
    std::string m_path;
    AssetHandle<Shader> m_shader;
    MaterialTextures m_textures;
    MaterialParams m_params;
    RenderState m_state;
    std::map<std::string, std::string, NumericComparator> m_info;
};

}
