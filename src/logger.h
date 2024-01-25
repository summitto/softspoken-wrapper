#ifndef EMP_OT_WRAPPER_LOGGER
#define EMP_OT_WRAPPER_LOGGER

#include <functional>
#include <string>

#include "optional.h"

using logger = std::function<void(const char*)>;

class Logger {
public:
  static void init(const logger& log) {
    Logger::m_log = log;
  }

  static void log(const char* logMessage) {
    Logger::m_log(logMessage);
  }

  static void log(const std::string& logMessage) {
    Logger::m_log(logMessage.c_str());
  }

private:
  static optional<logger> m_log;
};

#endif // EMP_OT_WRAPPER_LOGGER
