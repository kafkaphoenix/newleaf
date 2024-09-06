#include "log_manager.h"

#pragma warning(push, 0)
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#pragma warning(pop)

#include "../imgui/imgui_logsink.h"
#include <vector>

namespace nl {

void LogManager::init() {
  std::vector<spdlog::sink_ptr> logSinks;
  logSinks.reserve(2);
  logSinks.emplace_back(
    std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
  logSinks.emplace_back(std::make_shared<ImGuiLogsink>());

  logSinks[0]->set_pattern("%^[%T] %n: %v%$");

  s_engine_logger =
    std::make_shared<spdlog::logger>("engine", begin(logSinks), end(logSinks));
  spdlog::register_logger(s_engine_logger);
  s_engine_logger->set_level(s_engine_log_level);
  s_engine_logger->flush_on(s_engine_flush_level);

  s_app_logger =
    std::make_shared<spdlog::logger>("app", begin(logSinks), end(logSinks));
  spdlog::register_logger(s_app_logger);
  s_app_logger->set_level(s_app_log_level);
  s_app_logger->flush_on(s_app_flush_level);
}

void LogManager::create_file_logger(std::string_view path) {
  auto fileSink =
    std::make_shared<spdlog::sinks::basic_file_sink_mt>(path.data(), true);

  fileSink->set_pattern("[%D %T] [%l] %n: %v");

  s_engine_logger->sinks().emplace_back(fileSink);
  s_app_logger->sinks().emplace_back(fileSink);
}

void LogManager::create_backtrace_logger(std::string_view path,
                                         bool enable_engine_backtrace_logger,
                                         bool enable_app_backtrace_logger) {
  auto backtraceSink = std::make_shared<BacktraceLogsink>(path.data());
  s_engine_backtrace_logger =
    std::make_shared<spdlog::logger>("engine", backtraceSink);
  s_app_backtrace_logger =
    std::make_shared<spdlog::logger>("app", backtraceSink);

  if (enable_engine_backtrace_logger) {
    s_engine_backtrace_logger->set_level(spdlog::level::debug);
    s_engine_backtrace_logger->flush_on(spdlog::level::debug);
  } else {
    s_engine_backtrace_logger->set_level(spdlog::level::off);
    s_engine_backtrace_logger->flush_on(spdlog::level::off);
  }

  if (enable_app_backtrace_logger) {
    s_app_backtrace_logger->set_level(spdlog::level::debug);
    s_app_backtrace_logger->flush_on(spdlog::level::debug);
  } else {
    s_app_backtrace_logger->set_level(spdlog::level::off);
    s_app_backtrace_logger->flush_on(spdlog::level::off);
  }
}

void LogManager::clear_all_backtrace_logger() {
  auto sink = dynamic_cast<BacktraceLogsink*>(
    s_engine_backtrace_logger->sinks()[0].get());
  sink->clear("all");
}

void LogManager::clear_engine_backtrace_logger() {
  auto sink = dynamic_cast<BacktraceLogsink*>(
    s_engine_backtrace_logger->sinks()[0].get());
  sink->clear("engine");
}

void LogManager::clear_app_backtrace_logger() {
  auto sink =
    dynamic_cast<BacktraceLogsink*>(s_app_backtrace_logger->sinks()[0].get());
  sink->clear("app");
}

void LogManager::dump_backtrace() {
  auto sink = dynamic_cast<BacktraceLogsink*>(
    s_engine_backtrace_logger->sinks()[0].get());
  sink->dump_to_file();
}

void LogManager::set_engine_logger_level(spdlog::level::level_enum level) {
  s_engine_log_level = level;
  s_engine_logger->set_level(s_engine_log_level);
}

void LogManager::set_app_logger_level(spdlog::level::level_enum level) {
  s_app_log_level = level;
  s_app_logger->set_level(s_app_log_level);
}

void LogManager::set_engine_logger_flush_level(
  spdlog::level::level_enum level) {
  s_engine_flush_level = level;
  s_engine_logger->flush_on(level);
}

void LogManager::set_app_logger_flush_level(spdlog::level::level_enum level) {
  s_app_flush_level = level;
  s_app_logger->flush_on(level);
}

void LogManager::toggle_engine_logger(bool enable) {
  if (enable and not is_engine_logger_enabled()) {
    s_engine_logger->set_level(s_engine_log_level);
    s_engine_logger->flush_on(s_engine_flush_level);
  } else if (not enable and is_engine_logger_enabled()) {
    s_engine_logger->set_level(spdlog::level::off);
    s_engine_logger->flush_on(spdlog::level::off);
  }
}

void LogManager::toggle_app_logger(bool enable) {
  if (enable and not is_app_logger_enabled()) {
    s_app_logger->set_level(s_app_log_level);
    s_app_logger->flush_on(s_app_flush_level);
  } else if (not enable and is_app_logger_enabled()) {
    s_app_logger->set_level(spdlog::level::off);
    s_app_logger->flush_on(spdlog::level::off);
  }
}

void LogManager::toggle_engine_backtrace_logger(bool enable) {
  if (enable and not is_engine_backtrace_logger_enabled()) {
    s_engine_backtrace_logger->set_level(spdlog::level::debug);
    s_engine_backtrace_logger->flush_on(spdlog::level::debug);
  } else if (not enable and is_engine_backtrace_logger_enabled()) {
    s_engine_backtrace_logger->set_level(spdlog::level::off);
    s_engine_backtrace_logger->flush_on(spdlog::level::off);
  }
}

void LogManager::toggle_app_backtrace_logger(bool enable) {
  if (enable and not is_app_backtrace_logger_enabled()) {
    s_app_backtrace_logger->set_level(spdlog::level::debug);
    s_app_backtrace_logger->flush_on(spdlog::level::debug);
  } else if (not enable and is_app_backtrace_logger_enabled()) {
    s_app_backtrace_logger->set_level(spdlog::level::off);
    s_app_backtrace_logger->flush_on(spdlog::level::off);
  }
}

}