#pragma once

#include <entt/entt.hpp>

#include "../../logging/log_manager.h"
#include "../../utils/assert.h"
#include "../../utils/numeric_comparator.h"

#include <map>
#include <string>

namespace nl {

struct CTime {
    float seconds{};
    uint32_t current_hour{};
    uint32_t current_minute{};
    uint32_t current_second{};
    float day_length{0.5};
    float starting_time{7};
    float day_transition_start{5};
    float day_start{7};
    float night_transition_start{19};
    float night_start{21};
    float acceleration{1.f};
    uint32_t fps{60};

    CTime() = default;
    explicit CTime(float s, uint32_t ch, uint32_t cm, uint32_t cs, float st,
                   float dl, float ns, float dts, float ds, float nts, float a,
                   uint32_t f)
      : seconds(s), current_hour(ch), current_minute(cm), current_second(cs),
        starting_time(st), day_length(dl), night_start(ns),
        day_transition_start(dts), day_start(ds), night_transition_start(nts),
        acceleration(a), fps(f) {}

    void print() const {
      ENGINE_BACKTRACE(
        "\t\tseconds: {0}\n\t\t\t\t\t\tcurrent_hour: "
        "{1}\n\t\t\t\t\t\tcurrent_minute: {2}\n\t\t\t\t\t\tcurrent_second: "
        "{3}\n\t\t\t\t\t\tday_length: {4}\n\t\t\t\t\t\tstarting_time: "
        "{5}\n\t\t\t\t\t\tday_transition_start: {6}\n\t\t\t\t\t\tday_start: "
        "{7}\n\t\t\t\t\t\tnight_transition_start: "
        "{8}\n\t\t\t\t\t\tnight_start: {9}\n\t\t\t\t\t\tacceleration: "
        "{10}\n\t\t\t\t\t\tfps: {11}",
        seconds, current_hour, current_minute, current_second, day_length,
        starting_time, day_transition_start, day_start, night_transition_start,
        night_start, acceleration, fps);
    }

    std::map<std::string, std::string, NumericComparator> to_map() const {
      std::map<std::string, std::string, NumericComparator> info;
      info["seconds"] = std::to_string(seconds);
      info["current_hour"] = std::to_string(current_hour);
      info["current_minute"] = std::to_string(current_minute);
      info["current_second"] = std::to_string(current_second);
      info["day_length"] = std::to_string(day_length);
      info["starting_time"] = std::to_string(starting_time);
      info["day_transition_start"] = std::to_string(day_transition_start);
      info["day_start"] = std::to_string(day_start);
      info["night_transition_start"] = std::to_string(night_transition_start);
      info["night_start"] = std::to_string(night_start);
      info["acceleration"] = std::to_string(acceleration);
      info["fps"] = std::to_string(fps);

      return info;
    }

    void set_time(float time) {
      seconds = time * 3600.f;
      current_hour = static_cast<uint32_t>(seconds / 3600.f);
      current_minute =
        static_cast<uint32_t>((seconds / 60.f) - (current_hour * 60.f));
      current_second = static_cast<uint32_t>(seconds) % 60;
    }

    void validate() {
      ENGINE_ASSERT(day_length > 0.f, "day length must be positive!");
      ENGINE_ASSERT(starting_time > 0.f, "starting time must be positive!");
      ENGINE_ASSERT(night_start > 0.f, "night start must be positive!");
      ENGINE_ASSERT(day_transition_start > 0.f,
                    "day transition start must be positive!");
      ENGINE_ASSERT(day_start > 0.f, "day start must be positive!");
      ENGINE_ASSERT(night_transition_start > 0.f,
                    "night transition start must be positive!");
      ENGINE_ASSERT(acceleration > 0.f, "acceleration must be positive!");
      ENGINE_ASSERT(fps > 0, "fps must be positive!");
    }
};
}

template <>
inline void nl::SceneManager::on_component_added(entt::entity e, CTime& c) {
  c.validate();
  c.set_time(c.starting_time);

  m_registry.replace<CTime>(e, c);
}