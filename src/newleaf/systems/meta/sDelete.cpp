#include "sDelete.h"

#include "../../application/application.h"
#include "../../components/meta/cDeleted.h"
#include "../../components/meta/cUUID.h"

namespace nl {

void DeleteSystem::update(entt::registry& registry, const Time& ts) {
  if (Application::get().is_paused()) {
    return;
  }

  auto to_destroy = registry.view<CDeleted, CUUID>();
  registry.destroy(to_destroy.begin(), to_destroy.end());
}
}