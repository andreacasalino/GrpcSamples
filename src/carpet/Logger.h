#pragma once

#include <SpinLockGuard.h>
#include <Strings.h>

#include <iostream>
#include <unordered_map>

namespace carpet {
struct TimeNow {
  static TimeNow get();

  int hours;
  int minutes;
  int seconds;
};

std::ostream &operator<<(std::ostream &, const TimeNow &subject);

class Logger {
public:
  static Logger &get();

  enum class Severity { ERROR, WARNING, INFO };

  template <typename... ARGS>
  void log(Severity severity, const std::string &filename, int line,
           ARGS &&...args) {
    SpinLockGuard guard{lock};
    Strings::joinStream<' '>(std::cout, TimeNow::get(), '|',
                             preambles[severity], '|',
                             std::forward<ARGS>(args)...,
                             filename + ":" + std::to_string(line), '\n');
  }

private:
  Logger();

  std::atomic_bool lock = true;
  std::unordered_map<Severity, const char *> preambles;
};

} // namespace carpet

#define LOGE(...)                                                              \
  trd::Logger::get().log(trd::Logger::Severity::ERROR, __FILE__, __LINE__,     \
                         __VA_ARGS__);
#define LOGW(...)                                                              \
  trd::Logger::get().log(trd::Logger::Severity::WARNING, __FILE__, __LINE__,   \
                         __VA_ARGS__);
#define LOGI(...)                                                              \
  trd::Logger::get().log(trd::Logger::Severity::INFO, __FILE__, __LINE__,      \
                         __VA_ARGS__);
