#pragma once

#include <entt/entt.hpp>

#include "../../assets/texture.h"
#include "../../utils/time.h"
#include "../system.h"

namespace nl {

class SkyboxSystem : public System {
  public:
    SkyboxSystem(int priority) : System(priority) {}

    void update(entt::registry& registry, const Time& ts) override final;
};

}
