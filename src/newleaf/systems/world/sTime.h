#pragma once

#include <entt/entt.hpp>

#include "../../utils/time.h"
#include "../system.h"

namespace nl {

class TimeSystem : public System {
  public:
    TimeSystem(int priority) : System(priority) {}

    void update(entt::registry& registry, const Time& ts) override final;
};

}
