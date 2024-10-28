#pragma once

#include <memory>
#include <string_view>

#pragma warning(push, 0)
#include <spdlog/spdlog.h>
#pragma warning(pop)

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
    static void set_engine_logger_level(uint32_t level);
    static void set_app_logger_level(uint32_t level);
    static void set_engine_logger_flush_level(uint32_t level);
    static void set_app_logger_flush_level(uint32_t level);
    static void toggle_engine_logger(bool enable);
    static void toggle_app_logger(bool enable);
    static void toggle_engine_backtrace_logger(bool enable);
    static void toggle_app_backtrace_logger(bool enable);
    static spdlog::logger& get_engine_logger() {
      return *m_engine_logger;
    }
    static spdlog::logger& get_app_logger() {
      return *m_app_logger;
    }
    static spdlog::logger& get_engine_backtrace_logger() {
      return *m_engine_backtrace_logger;
    }
    static spdlog::logger& get_app_backtrace_logger() {
      return *m_app_backtrace_logger;
    }
    static std::string_view get_engine_logger_level() {
      return std::string_view(
        spdlog::level::to_string_view(m_engine_logger->level()));
    }
    static std::string_view get_app_logger_level() {
      return std::string_view(
        spdlog::level::to_string_view(m_app_logger->level()));
    }
    static std::string_view get_engine_logger_flush_level() {
      return std::string_view(
        spdlog::level::to_string_view(m_engine_logger->flush_level()));
    }
    static std::string_view get_app_logger_flush_level() {
      return std::string_view(
        spdlog::level::to_string_view(m_app_logger->flush_level()));
    }
    static bool is_engine_logger_enabled() {
      return m_engine_logger->level() != OFF;
    }
    static bool is_app_logger_enabled() { return m_app_logger->level() != OFF; }
    static bool is_engine_backtrace_logger_enabled() {
      return m_engine_backtrace_logger->level() != OFF;
    }
    static bool is_app_backtrace_logger_enabled() {
      return m_app_backtrace_logger->level() != OFF;
    }

    static constexpr spdlog::level::level_enum TRACE = spdlog::level::trace;
    static constexpr spdlog::level::level_enum DEBUG = spdlog::level::debug;
    static constexpr spdlog::level::level_enum INFO = spdlog::level::info;
    static constexpr spdlog::level::level_enum WARN = spdlog::level::warn;
    static constexpr spdlog::level::level_enum ERR = spdlog::level::err;
    static constexpr spdlog::level::level_enum CRITICAL =
      spdlog::level::critical;
    static constexpr spdlog::level::level_enum OFF = spdlog::level::off;

  private:
    inline static std::shared_ptr<spdlog::logger> m_engine_logger;
    inline static std::shared_ptr<spdlog::logger> m_app_logger;
    inline static std::shared_ptr<spdlog::logger> m_engine_backtrace_logger;
    inline static std::shared_ptr<spdlog::logger> m_app_backtrace_logger;
    inline static enum ::spdlog::level::level_enum m_engine_log_level = TRACE;
    inline static enum ::spdlog::level::level_enum m_app_log_level = TRACE;
    inline static enum ::spdlog::level::level_enum m_engine_flush_level = TRACE;
    inline static enum ::spdlog::level::level_enum m_app_flush_level = TRACE;
};

}

#define ENGINE_TRACE(...)                                                      \
  ::nl::LogManager::get_engine_logger().trace(__VA_ARGS__)
#define ENGINE_DEBUG(...)                                                      \
  ::nl::LogManager::get_engine_logger().debug(__VA_ARGS__)
#define ENGINE_INFO(...)                                                       \
  ::nl::LogManager::get_engine_logger().info(__VA_ARGS__)
#define ENGINE_WARN(...)                                                       \
  ::nl::LogManager::get_engine_logger().warn(__VA_ARGS__)
#define ENGINE_ERROR(...)                                                      \
  ::nl::LogManager::get_engine_logger().error(__VA_ARGS__)
#define ENGINE_CRITICAL(...)                                                   \
  ::nl::LogManager::get_engine_logger().critical(__VA_ARGS__)

#define APP_TRACE(...) ::nl::LogManager::get_app_logger().trace(__VA_ARGS__)
#define APP_DEBUG(...) ::nl::LogManager::get_app_logger().debug(__VA_ARGS__)
#define APP_INFO(...) ::nl::LogManager::get_app_logger().info(__VA_ARGS__)
#define APP_WARN(...) ::nl::LogManager::get_app_logger().warn(__VA_ARGS__)
#define APP_ERROR(...) ::nl::LogManager::get_app_logger().error(__VA_ARGS__)
#define APP_CRITICAL(...)                                                      \
  ::nl::LogManager::get_app_logger().critical(__VA_ARGS__)

#define ENGINE_BACKTRACE(...)                                                  \
  ::nl::LogManager::get_engine_backtrace_logger().debug(__VA_ARGS__)
#define APP_BACKTRACE(...)                                                     \
  ::nl::LogManager::get_app_backtrace_logger().debug(__VA_ARGS__)