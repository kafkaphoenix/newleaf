#pragma once

#include "../window/key_codes.h"
#include "event.h"

namespace nl {

class KeyEvent : public Event {
  public:
    Key get_key() const { return m_key; }

    EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)

  protected:
    KeyEvent(const Key key) : m_key(key) {}

    Key m_key{};
};

class KeyPressedEvent : public KeyEvent {
  public:
    KeyPressedEvent(const Key key, bool is_repeating)
      : KeyEvent(key), m_repeating(is_repeating) {}

    bool is_repeating() const { return m_repeating; }

    EVENT_CLASS_TYPE(KeyPressed)

  private:
    bool m_repeating{};
};

class KeyReleasedEvent : public KeyEvent {
  public:
    KeyReleasedEvent(const Key key) : KeyEvent(key) {}

    EVENT_CLASS_TYPE(KeyReleased)
};

class KeyTypedEvent : public KeyEvent {
  public:
    KeyTypedEvent(const Key key) : KeyEvent(key) {}

    EVENT_CLASS_TYPE(KeyTyped)
};

}