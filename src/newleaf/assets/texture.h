#pragma once

#include <glad/glad.h>

#include <filesystem>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include "../utils/numeric_comparator.h"
#include "asset.h"

namespace nl::assets {
class Texture : public Asset {
  public:
    Texture() = default; // TODO revisit is for std::vector in Model class
    Texture(uint32_t width, uint32_t height, GLenum glFormat,
            std::optional<bool> wrap = std::nullopt);
    Texture(std::filesystem::path&& fp,
            std::optional<std::string>&& type = std::nullopt,
            std::optional<bool> flipVertically = std::nullopt,
            std::optional<uint32_t> mipmap_level = std::nullopt,
            std::optional<bool> gammaCorrection = std::nullopt);
    virtual ~Texture() override final;

    void bind_slot(uint32_t slot);
    void rebind_slot();
    void unbind_slot();

    uint32_t get_width() const { return m_width; }
    uint32_t get_height() const { return m_height; }
    uint32_t get_id() const { return m_id; }
    std::string_view get_path() const {
      return (m_paths.size() == 1) ? m_paths[0] : m_directory;
    }
    std::string_view get_type() const { return m_type; }
    virtual const std::map<std::string, std::string, NumericComparator>&
    to_map() override final;
    bool is_cubemap() const { return m_cubemap; }

    virtual bool operator==(const Asset& other) const override final;

    static constexpr bool FLIP_VERTICALLY = true;
    static constexpr bool DONT_FLIP_VERTICALLY = false;
    static constexpr bool GAMMA_CORRECTION = true;
    static constexpr bool WRAP = true;
    static constexpr bool DONT_WRAP = false;

    static std::unique_ptr<Texture> Create(uint32_t width, uint32_t height,
                                           GLenum glFormat,
                                           std::optional<bool> wrap);

  private:
    std::vector<std::string> m_paths;
    std::string m_directory;
    std::string m_type;
    uint32_t m_width{}, m_height{};
    uint32_t m_id{};
    GLenum m_opengl_format{}, m_format{};
    uint32_t m_slot{};
    bool m_cubemap{};
    bool m_flip_vertically{true};
    uint32_t m_mipmap_level{};
    bool m_gamma_correction{};

    std::map<std::string, std::string, NumericComparator> m_info;

    void loadTexture();
};
}