#pragma once

#include "events/event.h"

namespace nl::events {

class AppTickEvent : public Event {
  public:
    EVENT_CLASS_TYPE(AppTick)
    EVENT_CLASS_CATEGORY(EventCategoryApplication)
};

class AppUpdateEvent : public Event {
  public:
    AppUpdateEvent(std::string&& dispatcher_target)
      : m_dispatcher_target(dispatcher_target) {}

    std::string_view get_dispatcher_target() const {
      return m_dispatcher_target;
    }

    EVENT_CLASS_TYPE(AppUpdate)
    EVENT_CLASS_CATEGORY(EventCategoryApplication)

  private:
    std::string m_dispatcher_target;
};

class AppRenderEvent : public Event {
  public:
    EVENT_CLASS_TYPE(AppRender)
    EVENT_CLASS_CATEGORY(EventCategoryApplication)
};
}