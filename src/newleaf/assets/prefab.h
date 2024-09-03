#pragma once

#include <nlohmann/json.hpp>

#include "../assets/asset.h"
#include "../pch.h"
#include "../utils/numeric_comparator.h"

using json = nlohmann::json;

struct Prototype {
    std::vector<std::string> inherits;
    std::vector<std::string> ctags;
    std::unordered_map<std::string, json> components;
};

namespace nl::assets {
class Prefab : public Asset {
  public:
    Prefab(std::filesystem::path&& fp,
           std::vector<std::string>&& targetedPrototypes);

    virtual const std::map<std::string, std::string, NumericComparator>&
    to_map() override final;
    const std::map<std::string, std::string, NumericComparator>&
    get_targeted_prototype_info(std::string_view prototype_id);

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

    const std::vector<std::string>& get_targeted_prototypes() const {
      return m_targeted_prototypes;
    }

    std::string_view get_name() const { return m_name; }

    const std::unordered_map<std::string, Prototype>& get_prototypes() const {
      return m_prototypes;
    }

    virtual bool operator==(const Asset& other) const override final;

  private:
    std::string m_name;
    std::string m_path;
    std::vector<std::string> m_targeted_prototypes;
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