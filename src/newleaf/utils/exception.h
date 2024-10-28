#pragma once

#include <stdexcept>
#include <string>

namespace nl {

class CustomException : public std::runtime_error {
  public:
    CustomException(const std::string& arg, const char* file, int line);

    const char* what() const override;

    ~CustomException();

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

#define throw_engine_exception(arg)                                            \
  throw ::nl::EngineException(std::format(arg), __FILE__, __LINE__)
#define throw_app_exception(arg)                                               \
  throw ::nl::AppException(std::format(arg), __FILE__, __LINE__)
