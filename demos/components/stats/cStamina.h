#pragma once

#include "engineAPI.h"

namespace demos {

struct CStamina {
    int base{};
    int current{};

    CStamina() = default;
    explicit CStamina(int b) : base(b), current(b) {}

    void print() const {
      APP_BACKTRACE("\t\tbase: {0}\n\t\t\t\tcurrent: {1}", base, current);
    }

    std::map<std::string, std::string, engine::NumericComparator> getInfo() const {
      std::map<std::string, std::string, engine::NumericComparator> info;
      info["base"] = std::to_string(base);
      info["current"] = std::to_string(current);

      return info;
    }
};
}