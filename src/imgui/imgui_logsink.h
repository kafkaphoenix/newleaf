#pragma once

#include <imgui.h>
#pragma warning(push, 0)
#include <spdlog/sinks/base_sink.h>
#include <spdlog/spdlog.h>
#pragma warning(pop)

#include <shared_mutex> // read-write lock (many readers allowed, but writing must be exclusive)
#include <string>
#include <vector>

namespace potatoengine {
class ImGuiLogsink : public spdlog::sinks::base_sink<std::mutex> {
  public:
    void draw(bool* show_tool_logger);
    void clear();

  protected:
    void sink_it_(const spdlog::details::log_msg& msg) override final;
    void flush_() override final;

  private:
    void show_log_format_popup();
    struct Record {
        std::string m_time;
        std::string m_thread;
        std::string m_source;
        std::string m_level;
        std::string m_msg;
    };
    int calculate_elapsed_local_time(const Record& r);
    std::vector<Record> m_records;
    mutable std::shared_timed_mutex m_records_mutex;

    bool m_autoscroll{};
    bool m_wrap{};

    bool m_show_time{true};
    bool m_show_thread{true};
    bool m_show_source{true};
    bool m_show_level{true};
    std::vector<std::string> m_levels{"all",     "trace", "debug",   "info",
                                      "warning", "error", "critical"};
    std::string m_filter_level{"all"};
    std::vector<std::string> m_times{"all", "1s",  "5s",  "10s", "30s",
                                     "1m",  "5m",  "10m", "30m", "1h",
                                     "5h",  "10h", "30h"};
    std::string m_filter_time{"all"};
    char m_text_filter[128]{};
};
}