#pragma once

#include "../events/event.h"

namespace nl::events {

class MouseMovedEvent : public Event {
  public:
    MouseMovedEvent(float x, float y) : m_mouse_x(x), m_mouse_y(y) {}

    float get_x() const { return m_mouse_x; }
    float get_y() const { return m_mouse_y; }

    EVENT_CLASS_TYPE(MouseMoved)
    EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)

  private:
    float m_mouse_x{}, m_mouse_y{};
};

class MouseScrolledEvent : public Event {
  public:
    MouseScrolledEvent(float x, float y) : m_mouse_x(x), m_mouse_y(y) {}

    float get_x() const { return m_mouse_x; }
    float get_y() const { return m_mouse_y; }

    EVENT_CLASS_TYPE(MouseScrolled)
    EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)

  private:
    float m_mouse_x{}, m_mouse_y{};
};

class MouseButtonEvent : public Event {
  public:
    uint32_t get_mouse_button() const { return m_button; }

    EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput |
                         EventCategoryMouseButton)

  protected:
    MouseButtonEvent(uint32_t b) : m_button(b) {}

    uint32_t m_button{};
};

class MouseButtonPressedEvent : public MouseButtonEvent {
  public:
    MouseButtonPressedEvent(uint32_t b) : MouseButtonEvent(b) {}

    EVENT_CLASS_TYPE(MouseButtonPressed)
};

class MouseButtonReleasedEvent : public MouseButtonEvent {
  public:
    MouseButtonReleasedEvent(uint32_t b) : MouseButtonEvent(b) {}

    EVENT_CLASS_TYPE(MouseButtonReleased)
};

}