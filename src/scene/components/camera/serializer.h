#pragma once

#include <nlohmann/json.hpp>

#include "scene/components/camera/cCamera.h"

using json = nlohmann::json;

namespace potatoengine { // TODO move or rethink this

inline void deserializeCamera(CCamera& cCamera, const json& options) {
  if (options.contains("type")) {
    cCamera._type = options.at("type").get<std::string>();
    cCamera.setCameraType();
  }
  if (options.contains("aspectRatio")) {
    cCamera._aspectRatio = options.at("aspectRatio").get<std::string>();
    cCamera.setAspectRatio();
  }
  if (options.contains("mode")) {
    cCamera._mode = options.at("mode").get<std::string>();
    cCamera.setMode();
  }
  if (options.contains("fov")) {
    cCamera.fov = options.at("fov").get<float>();
  }
  if (options.contains("zoomFactor")) {
    cCamera.zoomFactor = options.at("zoomFactor").get<float>();
  }
  if (options.contains("zoomMin")) {
    cCamera.zoomMin = options.at("zoomMin").get<float>();
  }
  if (options.contains("zoomMax")) {
    cCamera.zoomMax = options.at("zoomMax").get<float>();
  }
  if (options.contains("nearClip")) {
    cCamera.nearClip = options.at("nearClip").get<float>();
  }
  if (options.contains("farClip")) {
    cCamera.farClip = options.at("farClip").get<float>();
  }
  if (options.contains("cameraMode")) {
    cCamera._mode = options.at("cameraMode").get<std::string>();
    cCamera.setMode();
  }
}
}