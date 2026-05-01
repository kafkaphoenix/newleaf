#pragma once

#include <map>
#include <string>

#include "../utils/numeric_comparator.h"

namespace nl {

class Asset {
  public:
    virtual ~Asset() = default;
    virtual Asset& operator=(const Asset&) = delete;

    std::string_view get_uuid() const { return m_uuid; }
    void set_uuid(std::string uuid) { m_uuid = std::move(uuid); }

    virtual const std::map<std::string, std::string, NumericComparator>& to_map() = 0;
    virtual bool operator==(const Asset& other) const = 0;

  protected:
    std::string m_uuid;
};

}