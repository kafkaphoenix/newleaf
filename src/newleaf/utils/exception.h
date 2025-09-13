#pragma once

#include <stdexcept>
#include <string>

namespace nl {

class CustomException : public std::runtime_error {
  public:
    CustomException(const std::string& arg, const char* file, int line);

    const char* what() const override;

  private:
    std::string m_msg;
};

class EngineException : public CustomException {
  public:
    EngineException(const std::string& arg, const char* file, int line);
};

class AppException : public CustomException {
  public:
    AppException(const std::string& arg, const char* file, int line);
};

}

// format takes the string and variadic arguments to create the final message
#define throw_engine_exception(...) throw ::nl::EngineException(std::format(__VA_ARGS__), __FILE__, __LINE__)
#define throw_app_exception(...) throw ::nl::AppException(std::format(__VA_ARGS__), __FILE__, __LINE__)
