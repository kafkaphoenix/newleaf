#pragma once

#include "../pch.h"

namespace nl {

struct NumericComparator {
    bool operator()(const std::string& lhs, const std::string& rhs) const {
      if (lhs == rhs) {
        return false;
      }

      if (lhs.empty()) {
        return true;
      }

      if (rhs.empty()) {
        return false;
      }

      if (std::isdigit(lhs[0]) and not std::isdigit(rhs[0])) {
        return true;
      }

      if (!std::isdigit(lhs[0]) and std::isdigit(rhs[0])) {
        return false;
      }

      if (!std::isdigit(lhs[0]) and not std::isdigit(rhs[0])) {
        if (lhs[0] == rhs[0]) {
          return NumericComparator{}(lhs.substr(1), rhs.substr(1));
        }
        return lhs[0] < rhs[0];
      }

      // both lhs and rhs start with digit so we parse both numbers
      std::istringstream lhs_stream{lhs};
      std::istringstream rhs_stream{rhs};

      int lhs_number{};
      lhs_stream >> lhs_number;

      int rhs_number{};
      rhs_stream >> rhs_number;

      if (lhs_number == rhs_number) {
        return NumericComparator{}(lhs.substr(lhs_stream.tellg()),
                                   rhs.substr(rhs_stream.tellg()));
      }

      return lhs_number < rhs_number;
    }
};
}