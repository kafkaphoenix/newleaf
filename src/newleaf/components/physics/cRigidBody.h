#pragma once

#include "../../logging/log_manager.h"
#include "../../utils/numeric_comparator.h"

#include <map>
#include <string>

namespace nl {

struct CRigidBody {
    float mass{};
    float friction{};
    float bounciness{};
    bool kinematic{};

    CRigidBody() = default;
    explicit CRigidBody(float m, float f, float b, bool k)
      : mass(m), friction(f), bounciness(b), kinematic(k) {}

    void print() const {
      ENGINE_BACKTRACE(
        "\t\tmass: {0}\n\t\t\t\t\t\tfriction: "
        "{1}\n\t\t\t\t\t\tbounciness: {2}\n\t\t\t\t\t\tkinematic: {3}",
        mass, friction, bounciness, kinematic);
    }

    std::map<std::string, std::string, NumericComparator> to_map() const {
      std::map<std::string, std::string, NumericComparator> info;
      info["mass"] = std::to_string(mass);
      info["friction"] = std::to_string(friction);
      info["bounciness"] = std::to_string(bounciness);
      info["kinematic"] = kinematic ? "true" : "false";

      return info;
    }
};
}