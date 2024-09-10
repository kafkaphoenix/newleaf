#include "exception.h"

#include <format>
#include <sstream>
#include <iostream>

namespace nl {

CustomException::CustomException(const std::string& arg, const char* file,
                                 int line)
  : std::runtime_error(arg) {
  std::ostringstream o;
  o << file << ":" << line << ": " << arg;
  m_msg = o.str();
}

const char* CustomException::what() const { return m_msg.c_str(); }

EngineException::EngineException(const std::string& arg, const char* file,
                                 int line)
  : CustomException(arg, file, line) {}

AppException::AppException(const std::string& arg, const char* file, int line)
  : CustomException(arg, file, line) {}

}
