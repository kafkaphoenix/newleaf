#pragma once

#include <memory>
#include <string>

#include <entt/entt.hpp>

#include "../events/event.h"
#include "../utils/time.h"

namespace nl {

class System {
  public:
    System(int32_t priority = 0) : m_priority(priority) {}
    virtual ~System() = default;

    int32_t get_priority() const { return m_priority; }
    void update_priority(int32_t priority) { m_priority = priority; }

    virtual void init(entt::registry& registry) {};
    virtual void update(entt::registry& registry, const Time& ts) {};

  protected:
    int32_t m_priority = 0;
};

struct SystemPriorityComparator {
    bool operator()(const System* lhs, const System* rhs) const {
        return lhs->get_priority() < rhs->get_priority();
    }
};

}