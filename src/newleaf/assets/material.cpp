#include "material.h"
#include <format>
#include <glm/gtx/string_cast.hpp>

namespace nl {

Material::Material(AssetHandle<Shader> shader, const MaterialTextures& textures,
                   const MaterialParams& params, const RenderState& state)
  : m_shader(std::move(shader)), m_textures(textures), m_params(params), m_state(state) {}

const std::map<std::string, std::string, NumericComparator>& Material::to_map() {
  m_info["type"] = "Material";
  m_info["uuid"] = m_uuid;
  m_info["shader"] = m_shader.is_valid() ? m_shader.get()->get_uuid().data() : "undefined";

  m_info["blend"] = m_state.blend ? "true" : "false";
  m_info["depth_write"] = m_state.depth_write ? "true" : "false";
  m_info["cull"] = m_state.cull ? "true" : "false";

  m_info["base_color_texture"] =
    m_textures.base_color.is_valid() ? m_textures.base_color.get()->get_uuid().data() : "undefined";
  m_info["metallic_roughness_texture"] =
    m_textures.metallic_roughness.is_valid() ? m_textures.metallic_roughness.get()->get_uuid().data() : "undefined";
  m_info["normal_texture"] = m_textures.normal.is_valid() ? m_textures.normal.get()->get_uuid().data() : "undefined";
  m_info["emissive_texture"] =
    m_textures.emissive.is_valid() ? m_textures.emissive.get()->get_uuid().data() : "undefined";
  m_info["occlusion_texture"] =
    m_textures.occlusion.is_valid() ? m_textures.occlusion.get()->get_uuid().data() : "undefined";

  m_info["base_color_factor"] = glm::to_string(m_params.base_color_factor);
  m_info["metallic_factor"] = std::to_string(m_params.metallic_factor);
  m_info["roughness_factor"] = std::to_string(m_params.roughness_factor);
  m_info["emissive_factor"] = glm::to_string(m_params.emissive_factor);
  m_info["alpha_cutoff"] = std::to_string(m_params.alpha_cutoff);

  return m_info;
}

const std::map<std::string, std::string, NumericComparator>& Material::get_texture_info(std::string_view uuid) {
  if (m_textures.base_color.is_valid() && uuid == m_textures.base_color.get()->get_uuid()) {
    return m_textures.base_color.get()->to_map();
  } else if (m_textures.metallic_roughness.is_valid() && uuid == m_textures.metallic_roughness.get()->get_uuid()) {
    return m_textures.metallic_roughness.get()->to_map();
  } else if (m_textures.normal.is_valid() && uuid == m_textures.normal.get()->get_uuid()) {
    return m_textures.normal.get()->to_map();
  } else if (m_textures.emissive.is_valid() && uuid == m_textures.emissive.get()->get_uuid()) {
    return m_textures.emissive.get()->to_map();
  } else if (m_textures.occlusion.is_valid() && uuid == m_textures.occlusion.get()->get_uuid()) {
    return m_textures.occlusion.get()->to_map();
  }
  return std::map<std::string, std::string, NumericComparator>{
    {"error", std::format("texture with uuid {} not found in material", uuid)}};
}

bool Material::operator==(const Asset& other) const {
  if (typeid(other) != typeid(Material))
    return false;
  const Material& other_mat = static_cast<const Material&>(other);
  return m_uuid == other_mat.m_uuid && m_shader == other_mat.m_shader;
}

}
