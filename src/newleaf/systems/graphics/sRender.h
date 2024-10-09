#pragma once

#include <entt/entt.hpp>

#include "../../utils/time.h"
#include "../system.h"

namespace nl {

class RenderSystem : public System {
  public:
    RenderSystem(int priority) : System(priority) {}

    void update(entt::registry& registry, const Time& ts) override final;
};

}
