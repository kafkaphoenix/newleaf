#pragma once

#include <map>
#include <string>

#include "../../logging/log_manager.h"
#include "../../utils/numeric_comparator.h"
#include "../../application/application.h"
#include "../../assets/asset_handle.h"
#include "../../assets/assets_manager.h"
#include "../../scene/scene_manager.h"
#include "../../assets/shader.h"

namespace nl {

struct CShader {
    std::string uuid;
    AssetHandle<Shader> handle;
    bool visible{true};

    CShader() = default;
    explicit CShader(std::string&& uuid, bool visible = true) : uuid(std::move(uuid)), visible(visible) {}

    void print() const { ENGINE_BACKTRACE("\t\tuuid: {0}\n\t\t\t\t\t\tvisible: {1}", uuid, visible); }

    std::map<std::string, std::string, NumericComparator> to_map() const {
      std::map<std::string, std::string, NumericComparator> info;
      info["uuid"] = handle.get() ? handle.get()->get_uuid().data() : "undefined";
      info["visible"] = visible ? "true" : "false";

      return info;
    }

    void set_shader() {
      ENGINE_ASSERT(!uuid.empty(), "uuid for shader is empty");
      const auto& assets_manager = Application::get().get_assets_manager();
      handle = assets_manager.get<Shader>(uuid);
    }
};
}

template <> inline void nl::SceneManager::on_component_added(CShader& c) { c.set_shader(); }
