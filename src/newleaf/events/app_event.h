#pragma once

#include <string>
#include <string_view>

#include "event.h"

namespace nl {

class AppTickEvent : public Event {
  public:
    EVENT_CLASS_TYPE(AppTick)
    EVENT_CLASS_CATEGORY(EventCategoryApplication)
};

class AppUpdateEvent : public Event {
  public:
    AppUpdateEvent(std::string&& event_target) : m_event_target(event_target) {}

    std::string_view get_event_target() const { return m_event_target; }

    EVENT_CLASS_TYPE(AppUpdate)
    EVENT_CLASS_CATEGORY(EventCategoryApplication)

  private:
    std::string m_event_target;
};

class AppRenderEvent : public Event {
  public:
    EVENT_CLASS_TYPE(AppRender)
    EVENT_CLASS_CATEGORY(EventCategoryApplication)
};
}