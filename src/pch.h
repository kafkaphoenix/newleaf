#pragma once

#include <cctype>
#include <chrono>
#include <filesystem>
#include <format>
#include <fstream>
#include <functional>
#include <iosfwd>
#include <iterator>
#include <map>
#include <memory>
#include <random>
#include <ranges>
#include <span>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

#include "config.h" // makefile generated flags
#include "core/log_manager.h"
#include "utils/exception.h"

#define BIND_EVENT(f)                                                          \
  [this](auto&&... args) { return f(std::forward<decltype(args)>(args)...); }
#define BIND_STATIC_EVENT(f, ...)                                              \
  [&](auto&&... args) {                                                        \
    return f(std::forward<decltype(args)>(args)..., ##__VA_ARGS__);            \
  }
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