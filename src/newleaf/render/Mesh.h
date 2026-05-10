#pragma once

#include <format>
#include <map>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include <entt/entt.hpp>
#include <glm/glm.hpp>

#include "../logging/log_manager.h"
#include "vao.h"
#include "../utils/map_json_serializer.h"
#include "../utils/numeric_comparator.h"

using namespace entt::literals;

namespace nl {

class Mesh {
  public:
  Mesh() = default;
  explicit Mesh(std::unique_ptr<VAO>&& vao) : vao(std::move(vao)) {}
  
  Mesh(const Mesh&) = delete;
  Mesh& operator=(const Mesh&) = delete;
  Mesh(Mesh&&) noexcept = default;
  Mesh& operator=(Mesh&&) noexcept = default;
  
  VAO& get_vao() { return *vao; }
  const VAO& get_vao() const { return *vao; }
  
  
  void print() const {
    ENGINE_BACKTRACE("\t\tvertices: {0}\n\t\tindices: {1}", vao->get_vbo().get_count(), vao->get_ibo().get_count());
  }
  
  std::map<std::string, std::string, NumericComparator> to_map() const {
    std::map<std::string, std::string, NumericComparator> info;
    info["vao_0"] = vao ? get_vao_info() : "undefined";

    return info;
  }
  
  std::string get_vao_info() const { return map_to_json(vao->to_map()); }
  
  private:
    std::unique_ptr<VAO> vao;
};
}