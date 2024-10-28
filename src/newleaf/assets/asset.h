#pragma once

#include <map>
#include <string>

#include "../utils/numeric_comparator.h"

namespace nl {

class Asset {
  public:
    virtual ~Asset() = default;
    virtual Asset& operator=(const Asset&) = delete;

    virtual const std::map<std::string, std::string, NumericComparator>&
    to_map() = 0;
    virtual bool operator==(const Asset& other) const = 0;
};

}