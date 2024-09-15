#pragma once

#include "../window/key_codes.h"
#include "event.h"

namespace nl {

class KeyEvent : public Event {
  public:
    KeyCode get_key_code() const { return m_key_code; }

    EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)

  protected:
    KeyEvent(const KeyCode key_code) : m_key_code(key_code) {}

    KeyCode m_key_code{};
};

class KeyPressedEvent : public KeyEvent {
  public:
    KeyPressedEvent(const KeyCode key_code, bool is_repeating)
      : KeyEvent(key_code), m_repeating(is_repeating) {}

    bool is_repeating() const { return m_repeating; }

    EVENT_CLASS_TYPE(KeyPressed)

  private:
    bool m_repeating{};
};

class KeyReleasedEvent : public KeyEvent {
  public:
    KeyReleasedEvent(const KeyCode key_code) : KeyEvent(key_code) {}

    EVENT_CLASS_TYPE(KeyReleased)
};

class KeyTypedEvent : public KeyEvent {
  public:
    KeyTypedEvent(const KeyCode key_code) : KeyEvent(key_code) {}

    EVENT_CLASS_TYPE(KeyTyped)
};

}