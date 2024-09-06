#include "backtrace_logsink.h"

#include <chrono>
#include <filesystem>
#include <format>
#include <iostream>
#include <fstream>

extern template class spdlog::sinks::base_sink<std::mutex>;
namespace nl {

BacktraceLogsink::BacktraceLogsink(std::string&& path)
  : m_path(std::move(path)) {}

void BacktraceLogsink::dump_to_file() {
  std::unique_lock<std::shared_timed_mutex> lock(m_records_mutex);
  auto path = std::filesystem::path(m_path);
  if (!std::filesystem::exists(path.parent_path())) {
    std::filesystem::create_directories(path.parent_path());
  }
  if (std::filesystem::exists(path)) {
    std::filesystem::remove(path);
  }
  std::ofstream file(m_path);

  for (const auto& record : m_records) {
    if (!record.m_time.empty()) {
      file << record.m_time << " ";
    }
    if (!record.m_thread.empty()) {
      file << record.m_thread << " ";
    }
    if (!record.m_source.empty()) {
      file << record.m_source << " ";
    }
    if (!record.m_level.empty()) {
      file << record.m_level << " ";
    }
    file << record.m_msg << "\n";
  }
  lock.unlock();
}

void BacktraceLogsink::clear(std::string_view source) {
  std::unique_lock<std::shared_timed_mutex> lock(m_records_mutex);
  if (source == "all") {
    m_records.clear();
    lock.unlock();
    return;
  }

  m_records.erase(std::remove_if(m_records.begin(), m_records.end(),
                                 [&](const Record& record) {
                                   return record.m_source == source;
                                 }),
                  m_records.end());
  lock.unlock();
}

void BacktraceLogsink::sink_it_(const spdlog::details::log_msg& msg) {
  Record r;

  auto time = std::chrono::current_zone()->to_local(msg.time);
  r.m_time =
    std::format("{:%D %T}", std::chrono::floor<std::chrono::seconds>(time));

  r.m_thread = std::to_string(msg.thread_id);

  r.m_source = msg.logger_name.data();

  r.m_level = spdlog::level::to_string_view(msg.level).data();

  r.m_msg = std::string(msg.payload.data(), msg.payload.size());

  std::unique_lock<std::shared_timed_mutex> lock(m_records_mutex);
  m_records.emplace_back(std::move(r));
  lock.unlock();
}

void BacktraceLogsink::flush_() {}
}