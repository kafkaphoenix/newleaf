# newleaf

C++ game engine for 3d and 2d development

## Engine Features

- Scene manager: entity component system oriented. Loading scenes and entity prototypes from json
- Render manager: openGL 4.6 api abstraction. Model, texture and cubemaps loading capabilities. FBO
- Window manager: glfw abstraction
- Event-driven (mouse/keyboard/window/application)
- Settings manager: customizable engine defined settings
- Debugger layer: logger, metrics and an assets/entities/scene/states/settings inspector
- Assets manager: caching and hot reloading of prefabs, shaders, textures, models and scenes
- States manager: state machine with layers and overlays
- Perspective camera
- Model, texture and cubemaps loading capabilities

## How to use the engine in a personal project

- Use the engine as a library (shared or static)
```cmake
include(FetchContent)
FetchContent_Declare(
    newleaf
    GIT_REPOSITORY https://github.com/kafkaphoenix/newleaf.git
    GIT_TAG        v0.1.0
)

FetchContent_GetProperties(newleaf)
if(NOT newleaf_POPULATED)
  FetchContent_Populate(newleaf)
  set(BUILD_SHARED_LIBS ${BUILD_SHARED_LIBS} CACHE INTERNAL "" FORCE)
  add_subdirectory(${newleaf_SOURCE_DIR} ${newleaf_BINARY_DIR})
endif()
```

## Demos

- Engine use cases can be found in the [newleaf demos repository](https://github.com/kafkaphoenix/newleaf_demos)

## Planned features

- User defined settings
- Shader compilation
- Render commands batching
- Expand camera class supporting more modes
- Serialization
- Per module tests using catch2
- Profiler
- Font rendering
- Audio
- Physics
- Networking
- Vulkan support
- Engine wiki documentation
- Editor Mode
- Multi window/overlay
- Scripting language
- Multiplatform
- Multithreading

## Third party libraries

- [glfw](https://github.com/glfw/glfw): openGL application development framework
- [glad](https://github.com/Dav1dde/glad): openGL loader
- [EnTT](https://github.com/skypjack/entt): entity component system
- [stb](https://github.com/nothings/stb): image loading
- [glm](https://github.com/g-truc/glm): math operations
- [imGui](https://github.com/ocornut/imgui): ui framework
- [nlohmann_json](https://github.com/nlohmann/json): json processing
- [assimp](https://github.com/assimp/assimp): model loading
- [spdlog](https://github.com/gabime/spdlog): structured logging
- [Catch2](https://github.com/catchorg/Catch2): unit testing
