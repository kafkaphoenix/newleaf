#include "log_manager.h"

#include <vector>

#pragma warning(push, 0)
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#pragma warning(pop)

#include "backtrace_logsink.h"
#include "imgui_logsink.h"

namespace nl {

void LogManager::init() {
  std::vector<spdlog::sink_ptr> logSinks;
  logSinks.reserve(2);
  logSinks.emplace_back(
    std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
  logSinks.emplace_back(std::make_shared<ImGuiLogsink>());

  logSinks[0]->set_pattern("%^[%T] %n: %v%$");

  m_engine_logger =
    std::make_shared<spdlog::logger>("engine", begin(logSinks), end(logSinks));
  spdlog::register_logger(m_engine_logger);
  m_engine_logger->set_level(m_engine_log_level);
  m_engine_logger->flush_on(m_engine_flush_level);

  m_app_logger =
    std::make_shared<spdlog::logger>("app", begin(logSinks), end(logSinks));
  spdlog::register_logger(m_app_logger);
  m_app_logger->set_level(m_app_log_level);
  m_app_logger->flush_on(m_app_flush_level);
}

void LogManager::create_file_logger(std::string_view path) {
  auto fileSink =
    std::make_shared<spdlog::sinks::basic_file_sink_mt>(path.data(), true);

  fileSink->set_pattern("[%D %T] [%l] %n: %v");

  m_engine_logger->sinks().emplace_back(fileSink);
  m_app_logger->sinks().emplace_back(fileSink);
}

void LogManager::create_backtrace_logger(std::string_view path,
                                         bool enable_engine_backtrace_logger,
                                         bool enable_app_backtrace_logger) {
  auto backtraceSink = std::make_shared<BacktraceLogsink>(path.data());
  m_engine_backtrace_logger =
    std::make_shared<spdlog::logger>("engine", backtraceSink);
  m_app_backtrace_logger =
    std::make_shared<spdlog::logger>("app", backtraceSink);

  if (enable_engine_backtrace_logger) {
    m_engine_backtrace_logger->set_level(DEBUG);
    m_engine_backtrace_logger->flush_on(DEBUG);
  } else {
    m_engine_backtrace_logger->set_level(OFF);
    m_engine_backtrace_logger->flush_on(OFF);
  }

  if (enable_app_backtrace_logger) {
    m_app_backtrace_logger->set_level(DEBUG);
    m_app_backtrace_logger->flush_on(DEBUG);
  } else {
    m_app_backtrace_logger->set_level(OFF);
    m_app_backtrace_logger->flush_on(OFF);
  }
}

void LogManager::clear_all_backtrace_logger() {
  auto sink = dynamic_cast<BacktraceLogsink*>(
    m_engine_backtrace_logger->sinks()[0].get());
  sink->clear("all");
}

void LogManager::clear_engine_backtrace_logger() {
  auto sink = dynamic_cast<BacktraceLogsink*>(
    m_engine_backtrace_logger->sinks()[0].get());
  sink->clear("engine");
}

void LogManager::clear_app_backtrace_logger() {
  auto sink =
    dynamic_cast<BacktraceLogsink*>(m_app_backtrace_logger->sinks()[0].get());
  sink->clear("app");
}

void LogManager::dump_backtrace() {
  auto sink = dynamic_cast<BacktraceLogsink*>(
    m_engine_backtrace_logger->sinks()[0].get());
  sink->dump_to_file();
}

void LogManager::set_engine_logger_level(uint32_t level) {
  m_engine_log_level = static_cast<spdlog::level::level_enum>(level);
  m_engine_logger->set_level(m_engine_log_level);
}

void LogManager::set_app_logger_level(uint32_t level) {
  m_app_log_level = static_cast<spdlog::level::level_enum>(level);
  m_app_logger->set_level(m_app_log_level);
}

void LogManager::set_engine_logger_flush_level(uint32_t level) {
  m_engine_flush_level = static_cast<spdlog::level::level_enum>(level);
  m_engine_logger->flush_on(m_engine_flush_level);
}

void LogManager::set_app_logger_flush_level(uint32_t level) {
  m_app_flush_level = static_cast<spdlog::level::level_enum>(level);
  m_app_logger->flush_on(m_app_flush_level);
}

void LogManager::toggle_engine_logger(bool enable) {
  if (enable and not is_engine_logger_enabled()) {
    m_engine_logger->set_level(m_engine_log_level);
    m_engine_logger->flush_on(m_engine_flush_level);
  } else if (not enable and is_engine_logger_enabled()) {
    m_engine_logger->set_level(OFF);
    m_engine_logger->flush_on(OFF);
  }
}

void LogManager::toggle_app_logger(bool enable) {
  if (enable and not is_app_logger_enabled()) {
    m_app_logger->set_level(m_app_log_level);
    m_app_logger->flush_on(m_app_flush_level);
  } else if (not enable and is_app_logger_enabled()) {
    m_app_logger->set_level(OFF);
    m_app_logger->flush_on(OFF);
  }
}

void LogManager::toggle_engine_backtrace_logger(bool enable) {
  if (enable and not is_engine_backtrace_logger_enabled()) {
    m_engine_backtrace_logger->set_level(DEBUG);
    m_engine_backtrace_logger->flush_on(DEBUG);
  } else if (not enable and is_engine_backtrace_logger_enabled()) {
    m_engine_backtrace_logger->set_level(OFF);
    m_engine_backtrace_logger->flush_on(OFF);
  }
}

void LogManager::toggle_app_backtrace_logger(bool enable) {
  if (enable and not is_app_backtrace_logger_enabled()) {
    m_app_backtrace_logger->set_level(DEBUG);
    m_app_backtrace_logger->flush_on(DEBUG);
  } else if (not enable and is_app_backtrace_logger_enabled()) {
    m_app_backtrace_logger->set_level(OFF);
    m_app_backtrace_logger->flush_on(OFF);
  }
}

}