#include "sTime.h"

#include "../../application/application.h"
#include "../../components/meta/cUUID.h"
#include "../../components/world/cTime.h"
#include "../../events/app_event.h"
#include "../../window/windows_manager.h"

namespace nl {

void TimeSystem::update(entt::registry& registry, const Time& ts) {
  auto& app = Application::get();
  if (app.is_paused()) {
    return;
  }

  registry.view<CTime, CUUID>().each([&](CTime& cTime, const CUUID& cUUID) {
    // We call it fps frames per second instead of 1 but the other functions
    // divide by fps so it is ok
    float time_scale = 86400.f / (cTime.day_length * 3600.f);
    cTime.seconds += ts.get_seconds() * time_scale * cTime.acceleration;
    if (cTime.seconds >= 86400) {
      cTime.seconds = 0;
    }
    // dividing by fps because we call this function fps frames per second
    // instead of 1 but at the same time it helps us to see the time correctly
    // per frame so the logic works for 24 hours
    // TODO revisit probably will break with different fps
    uint32_t last_second = cTime.current_second;
    cTime.current_second = static_cast<uint32_t>(cTime.seconds) % cTime.fps;

    // tick every real second
    if (last_second != cTime.current_second) {
      // then we update minutes and hours
      if (cTime.current_second == 0) {
        app.get_window_manager()->trigger_event(AppTickEvent());

        cTime.current_minute++;
        if (cTime.current_minute == 60) {
          cTime.current_minute = 0;
          cTime.current_hour++;
          if (cTime.current_hour == 24) {
            cTime.current_hour = 0;
          }
        }
      }
    }
  });
}
}