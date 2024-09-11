#pragma once

#include <nlohmann/json.hpp>

#include <filesystem>
#include <map>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "../utils/numeric_comparator.h"
#include "asset.h"

using json = nlohmann::json;

struct Prototype {
    std::vector<std::string> inherits;
    std::vector<std::string> ctags;
    std::unordered_map<std::string, json> components;
};

namespace nl {
class Prefab : public Asset {
  public:
    Prefab(std::filesystem::path&& fp,
           std::vector<std::string>&& targetPrototypes);

    virtual const std::map<std::string, std::string, NumericComparator>&
    to_map() override final;
    const std::map<std::string, std::string, NumericComparator>&
    get_target_prototype_info(std::string_view prototype_id);

    const std::vector<std::string>&
    get_inherits(std::string_view prototype_id) const {
      return m_prototypes.at(prototype_id.data()).inherits;
    }

    const std::vector<std::string>&
    get_ctags(std::string_view prototype_id) const {
      return m_prototypes.at(prototype_id.data()).ctags;
    }

    const std::unordered_map<std::string, json>&
    get_components(std::string_view prototype_id) const {
      return m_prototypes.at(prototype_id.data()).components;
    }

    const std::vector<std::string>& get_target_prototypes() const {
      return m_target_prototypes;
    }

    std::string_view get_name() const { return m_name; }

    const std::unordered_map<std::string, Prototype>& get_prototypes() const {
      return m_prototypes;
    }

    virtual bool operator==(const Asset& other) const override final;

  private:
    std::string m_name;
    std::string m_path;
    std::vector<std::string> m_target_prototypes;
    std::unordered_map<std::string, Prototype> m_prototypes;

    std::map<std::string, std::string, NumericComparator> m_info;
    std::map<std::string, std::map<std::string, std::string, NumericComparator>,
             NumericComparator>
      m_prototype_info;

    void read(const json& j, std::vector<std::string>& inherits,
              std::vector<std::string>& ctags,
              std::unordered_map<std::string, json>& components);

    void process_prototype(const std::string& name, const json& prototype_data,
                           const json& data);
};
}