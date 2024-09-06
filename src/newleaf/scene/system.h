#pragma once

#include <cstdint>
#include <entt/entt.hpp>

#include "../core/time.h"
#include "../events/event.h"

#include <memory>
#include <string>

namespace nl::systems {

class System {
  public:
    System(int32_t priority = 0) : m_priority(priority) {}
    virtual ~System() = default;

    int32_t get_priority() const { return m_priority; }

    virtual void init(entt::registry& registry) {};
    virtual void update(entt::registry& registry, const Time& ts) {};

  protected:
    int32_t m_priority = 0;
};

struct SystemComparator {
    bool operator()(
      const std::pair<std::string, std::unique_ptr<System>>& lhs,
      const std::pair<std::string, std::unique_ptr<System>>& rhs) const {
      return lhs.second->get_priority() < rhs.second->get_priority();
    }
};

}