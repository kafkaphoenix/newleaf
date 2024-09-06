#pragma once

namespace nl::events {

enum class EventType {
  None = 0,
  AppTick,
  AppUpdate,
  AppRender,
  WindowClose,
  WindowResize,
  WindowRestored,
  WindowMinimized,
  WindowMaximized,
  WindowFocus,
  WindowLostFocus,
  WindowMoved,
  KeyPressed,
  KeyReleased,
  KeyTyped,
  MouseButtonPressed,
  MouseButtonReleased,
  MouseMoved,
  MouseScrolled
};

enum EventCategory {
  // Each category is a bit flag
  None = 0,
  EventCategoryApplication = 1,
  EventCategoryWindow = 2,
  EventCategoryInput = 4,
  EventCategoryKeyboard = 8,
  EventCategoryMouse = 16,
  EventCategoryMouseButton = 32,
};

#define EVENT_CLASS_TYPE(type)                                                 \
  static EventType get_static_type() { return EventType::type; }               \
  virtual EventType get_event_type() const override {                          \
    return get_static_type();                                                  \
  }                                                                            \
  virtual const char* get_name() const override { return #type; }

#define EVENT_CLASS_CATEGORY(category)                                         \
  virtual int get_category_flags() const override { return category; }

class Event {
  public:
    virtual ~Event() = default;
    bool is_handled{}; // cannnot be returned by a method

    virtual EventType get_event_type() const = 0;
    virtual const char* get_name() const = 0;
    virtual int get_category_flags() const = 0;

    bool is_in_category(EventCategory category) const {
      return get_category_flags() & category;
    }
};

class EventDispatcher {
  public:
    EventDispatcher(Event& e) : m_event(e) {}

    template <typename Type, typename Func> bool dispatch(const Func& func) {
      if (m_event.get_event_type() == Type::get_static_type()) {
        m_event.is_handled |= func(static_cast<Type&>(m_event));
        return true;
      }
      return false;
    }

  private:
    Event& m_event;
};

}

#define BIND_EVENT(f)                                                          \
  [this](auto&&... args) { return f(std::forward<decltype(args)>(args)...); }
#define BIND_STATIC_EVENT(f, ...)                                              \
  [&](auto&&... args) {                                                        \
    return f(std::forward<decltype(args)>(args)..., ##__VA_ARGS__);            \
  }