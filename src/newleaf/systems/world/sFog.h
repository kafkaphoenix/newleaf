#pragma once

#include <entt/entt.hpp>

#include "../../utils/time.h"
#include "../system.h"

namespace nl {

class FogSystem : public System {
  public:
    FogSystem(int priority) : System(priority) {}
    ~FogSystem() override final;

    void update(entt::registry& registry, const Time& ts) override final;
};

}
