#pragma once

#ifdef DEBUG
#define ENGINE_ASSERT(x, ...)                                                  \
  if (!(x)) {                                                                  \
    throw_engine_exception(__VA_ARGS__);                                       \
  }
#define APP_ASSERT(x, ...)                                                     \
  if (!(x)) {                                                                  \
    throw_app_exception(__VA_ARGS__);                                          \
  }
#else
#define ENGINE_ASSERT(x, ...)                                                  \
  if (!(x)) {                                                                  \
    ENGINE_CRITICAL(__VA_ARGS__);                                              \
  }
#define APP_ASSERT(x, ...)                                                     \
  if (!(x)) {                                                                  \
    APP_CRITICAL(__VA_ARGS__);                                                 \
  }
#endif