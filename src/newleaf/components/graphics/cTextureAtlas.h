#pragma once

#include <map>
#include <string>

#include "../../logging/log_manager.h"
#include "../../utils/numeric_comparator.h"

namespace nl {

struct CTextureAtlas {
    uint32_t rows{};
    uint32_t index{};

    CTextureAtlas() = default;
    explicit CTextureAtlas(uint32_t r, uint32_t i) : rows(r), index(i) {}

    void print() const {
      ENGINE_BACKTRACE("\t\trows: {0}\n\t\t\t\t\t\tindex: {1}", rows, index);
    }

    std::map<std::string, std::string, NumericComparator> to_map() const {
      std::map<std::string, std::string, NumericComparator> info;
      info["rows"] = std::to_string(rows);
      info["index"] = std::to_string(index);

      return info;
    }
};
}