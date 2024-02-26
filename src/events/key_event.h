#pragma once

#include "core/key_codes.h"
#include "events/event.h"

namespace nl::events {

class KeyEvent : public Event {
  public:
    KeyCode get_keycode() const { return m_keyCode; }

    EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)

  protected:
    KeyEvent(const KeyCode keycode) : m_keyCode(keycode) {}

    KeyCode m_keyCode{};
};

class KeyPressedEvent : public KeyEvent {
  public:
    KeyPressedEvent(const KeyCode keycode, bool is_repeating)
      : KeyEvent(keycode), m_repeating(is_repeating) {}

    bool is_repeating() const { return m_repeating; }

    EVENT_CLASS_TYPE(KeyPressed)

  private:
    bool m_repeating{};
};

class KeyReleasedEvent : public KeyEvent {
  public:
    KeyReleasedEvent(const KeyCode keycode) : KeyEvent(keycode) {}

    EVENT_CLASS_TYPE(KeyReleased)
};

class KeyTypedEvent : public KeyEvent {
  public:
    KeyTypedEvent(const KeyCode keycode) : KeyEvent(keycode) {}

    EVENT_CLASS_TYPE(KeyTyped)
};

}