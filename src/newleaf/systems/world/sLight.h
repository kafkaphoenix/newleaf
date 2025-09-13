#pragma once

#include <entt/entt.hpp>

#include "../../utils/time.h"
#include "../system.h"

namespace nl {

class LightSystem : public System {
  public:
    LightSystem(int priority) : System(priority) {}

    void update(entt::registry& registry, const Time& ts) override final;
};

}
