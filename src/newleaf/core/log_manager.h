#pragma once

#pragma warning(push, 0)
#include <spdlog/spdlog.h>
#pragma warning(pop)

#include "backtrace_logsink.h"
#include <memory>
#include <string_view>

namespace nl {

class LogManager {
  public:
    static void init();
    static void create_file_logger(std::string_view path);
    static void create_backtrace_logger(std::string_view path,
                                        bool enable_engine_backtrace_logger,
                                        bool enable_app_backtrace_logger);
    static void clear_all_backtrace_logger();
    static void clear_engine_backtrace_logger();
    static void clear_app_backtrace_logger();
    static void dump_backtrace();
    static void set_engine_logger_level(spdlog::level::level_enum level);
    static void set_app_logger_level(spdlog::level::level_enum level);
    static void set_engine_logger_flush_level(spdlog::level::level_enum level);
    static void set_app_logger_flush_level(spdlog::level::level_enum level);
    static void toggle_engine_logger(bool enable);
    static void toggle_app_logger(bool enable);
    static void toggle_engine_backtrace_logger(bool enable);
    static void toggle_app_backtrace_logger(bool enable);
    static std::shared_ptr<spdlog::logger>& get_engine_logger() {
      return s_engine_logger;
    }
    static std::shared_ptr<spdlog::logger>& get_app_logger() {
      return s_app_logger;
    }
    static std::shared_ptr<spdlog::logger>& get_engine_backtrace_logger() {
      return s_engine_backtrace_logger;
    }
    static std::shared_ptr<spdlog::logger>& get_app_backtrace_logger() {
      return s_app_backtrace_logger;
    }
    static std::string_view get_engine_logger_level() {
      return std::string_view(
        spdlog::level::to_string_view(s_engine_logger->level()));
    }
    static std::string_view get_app_logger_level() {
      return std::string_view(
        spdlog::level::to_string_view(s_app_logger->level()));
    }
    static std::string_view get_engine_logger_flush_level() {
      return std::string_view(
        spdlog::level::to_string_view(s_engine_logger->flush_level()));
    }
    static std::string_view get_app_logger_flush_level() {
      return std::string_view(
        spdlog::level::to_string_view(s_app_logger->flush_level()));
    }
    static bool is_engine_logger_enabled() {
      return s_engine_logger->level() != spdlog::level::off;
    }
    static bool is_app_logger_enabled() {
      return s_app_logger->level() != spdlog::level::off;
    }
    static bool is_engine_backtrace_logger_enabled() {
      return s_engine_backtrace_logger->level() != spdlog::level::off;
    }
    static bool is_app_backtrace_logger_enabled() {
      return s_app_backtrace_logger->level() != spdlog::level::off;
    }

  private:
    inline static std::shared_ptr<spdlog::logger> s_engine_logger;
    inline static std::shared_ptr<spdlog::logger> s_app_logger;
    inline static std::shared_ptr<spdlog::logger> s_engine_backtrace_logger;
    inline static std::shared_ptr<spdlog::logger> s_app_backtrace_logger;
    inline static enum ::spdlog::level::level_enum s_engine_log_level =
      spdlog::level::trace;
    inline static enum ::spdlog::level::level_enum s_app_log_level =
      spdlog::level::trace;
    inline static enum ::spdlog::level::level_enum s_engine_flush_level =
      spdlog::level::trace;
    inline static enum ::spdlog::level::level_enum s_app_flush_level =
      spdlog::level::trace;
};

}

#define ENGINE_TRACE(...)                                                      \
  ::nl::LogManager::get_engine_logger()->trace(__VA_ARGS__)
#define ENGINE_DEBUG(...)                                                      \
  ::nl::LogManager::get_engine_logger()->debug(__VA_ARGS__)
#define ENGINE_INFO(...)                                                       \
  ::nl::LogManager::get_engine_logger()->info(__VA_ARGS__)
#define ENGINE_WARN(...)                                                       \
  ::nl::LogManager::get_engine_logger()->warn(__VA_ARGS__)
#define ENGINE_ERROR(...)                                                      \
  ::nl::LogManager::get_engine_logger()->error(__VA_ARGS__)
#define ENGINE_CRITICAL(...)                                                   \
  ::nl::LogManager::get_engine_logger()->critical(__VA_ARGS__)

#define APP_TRACE(...) ::nl::LogManager::get_app_logger()->trace(__VA_ARGS__)
#define APP_DEBUG(...) ::nl::LogManager::get_app_logger()->debug(__VA_ARGS__)
#define APP_INFO(...) ::nl::LogManager::get_app_logger()->info(__VA_ARGS__)
#define APP_WARN(...) ::nl::LogManager::get_app_logger()->warn(__VA_ARGS__)
#define APP_ERROR(...) ::nl::LogManager::get_app_logger()->error(__VA_ARGS__)
#define APP_CRITICAL(...)                                                      \
  ::nl::LogManager::get_app_logger()->critical(__VA_ARGS__)

#define ENGINE_BACKTRACE(...)                                                  \
  ::nl::LogManager::get_engine_backtrace_logger()->debug(__VA_ARGS__)
#define APP_BACKTRACE(...)                                                     \
  ::nl::LogManager::get_app_backtrace_logger()->debug(__VA_ARGS__)

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