#pragma once

#include <glad/glad.h>

#include "assets/asset.h"
#include "pch.h"
#include "utils/numeric_comparator.h"

namespace potatoengine::assets {

class Shader : public Asset {
  public:
    Shader(std::filesystem::path&& fp);
    virtual ~Shader() override final;

    virtual const std::map<std::string, std::string, NumericComparator>&
    to_map() override final;

    virtual bool operator==(const Asset& other) const override final;
    operator GLuint() const { return m_id; }

  private:
    std::string m_path;
    uint32_t m_id{};
    GLenum m_type{};

    std::map<std::string, std::string, NumericComparator> m_info;
};

}