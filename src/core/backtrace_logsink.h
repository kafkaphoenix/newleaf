#pragma once

#pragma warning(push, 0)
#include <spdlog/sinks/base_sink.h>
#include <spdlog/spdlog.h>
#pragma warning(pop)

#include <shared_mutex> // read-write lock (many readers allowed, but writing must be exclusive)
#include <string>
#include <vector>

namespace nl {

class BacktraceLogsink : public spdlog::sinks::base_sink<std::mutex> {
  public:
    BacktraceLogsink(std::string&& path);
    void dump_to_file();
    void clear(std::string_view source);

  protected:
    void sink_it_(const spdlog::details::log_msg& msg) override final;
    void flush_() override final;

  private:
    struct Record {
        std::string m_time;
        std::string m_thread;
        std::string m_source;
        std::string m_level;
        std::string m_msg;
    };
    std::vector<Record> m_records;
    mutable std::shared_timed_mutex m_records_mutex;

    std::string m_path;
};
}