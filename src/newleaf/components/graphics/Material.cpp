#include "Material.h"

namespace nl {

Material::Material(std::string name, AssetHandle<Shader> shader, const MaterialTextures& textures,
                   const MaterialParams& params, const RenderState& state)
  : m_path(std::move(name)), m_shader(std::move(shader)), m_textures(textures), m_params(params), m_state(state) {}

const std::map<std::string, std::string, NumericComparator>& Material::to_map() {
  m_info["type"] = "Material";
  m_info["path"] = m_path;
  return m_info;
}

bool Material::operator==(const Asset& other) const {
  if (typeid(other) != typeid(Material))
    return false;
  const Material& other_mat = static_cast<const Material&>(other);
  return m_path == other_mat.m_path && m_shader == other_mat.m_shader;
}

}
