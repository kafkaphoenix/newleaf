#pragma once

#include <filesystem>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include <glad/glad.h>

#include "../utils/numeric_comparator.h"
#include "asset.h"

namespace nl {
class Texture : public Asset {
  public:
    Texture() = delete;
    Texture(uint32_t width, uint32_t height, GLenum glFormat, std::optional<bool> wrap = std::nullopt);
    Texture(std::filesystem::path&& fp, std::optional<bool> flip_vertically = std::nullopt,
            std::optional<bool> gamma_correction = std::nullopt);
    ~Texture() override final;

    void bind(uint32_t slot) const;

    uint32_t get_width() const { return m_width; }
    uint32_t get_height() const { return m_height; }
    uint32_t get_id() const { return m_id; }
    std::string_view get_path() const { return (m_paths.size() == 1) ? m_paths[0] : m_directory; }
    const std::map<std::string, std::string, NumericComparator>& to_map() override final;
    bool is_cubemap() const { return m_cubemap; }

    bool operator==(const Asset& other) const override final;

    static constexpr bool FLIP_VERTICALLY = true;
    static constexpr bool DONT_FLIP_VERTICALLY = false;
    static constexpr bool GAMMA_CORRECTION = true;
    static constexpr bool WRAP = true;
    static constexpr bool DONT_WRAP = false;

    static std::unique_ptr<Texture> create(uint32_t width, uint32_t height, GLenum glFormat, std::optional<bool> wrap);

  private:
    std::vector<std::string> m_paths;
    std::string m_directory;
    uint32_t m_width{}, m_height{};
    uint32_t m_id{};
    GLenum m_opengl_format{}, m_format{};
    bool m_cubemap{};
    bool m_flip_vertically{true};
    uint32_t m_mipmap_level{};
    bool m_gamma_correction{};

    std::map<std::string, std::string, NumericComparator> m_info;

    void setup_2d_params();
    void setup_cubemap_params();
    std::vector<std::string> default_cubemap_paths(const std::filesystem::path& directory, const std::string& ext);
    int calc_mipmap_levels(int width, int height);
};
}