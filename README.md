# newleaf

C++ game engine for 3D and 2D development

## Engine Features

- Scene manager: Entity Component System oriented. Loading scenes and entity prototypes from json
- Render manager: OpenGL 4.6 API abstraction. Model, Texture and Cubemaps loading capabilities. FBO
- Window manager: GLFW abstraction
- Event-driven (Mouse/Keyboard/Window/Application)
- Settings manager: Customizable engine defined settings
- Debugger layer: Logger, Metrics and a assets/entities/scene/states/settings inspector
- Assets manager: Caching and hot reloading of prefabs, shaders, textures, models and scenes
- States manager: State machine with layers and overlays
- Perspective camera
- Model, Texture and Cubemaps loading capabilities

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

- Engine use cases can be found in the [demos repo](https://github.com/kafkaphoenix/newleaf_demos)

## Planned features

- User defined settings
- Shader compilation
- Render commands batching
- Expand camera class supporting more modes
- Serialization
- Per module tests using Catch2
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
