#include <Logger.h>

#include <ctime>
#include <time.h>

namespace trd {
Logger &Logger::get() {
  static Logger res = Logger{};
  return res;
}

Logger::Logger()
    : preambles{{Severity::ERROR, " ERR  "},
                {Severity::WARNING, " WARN "},
                {Severity::INFO, " INFO "}} {}

TimeNow TimeNow::get() {
  auto res = std::time(NULL);
  auto parsed = std::localtime(&res);
  TimeNow retVal;
  retVal.hours = parsed->tm_hour;
  retVal.minutes = parsed->tm_min;
  retVal.seconds = parsed->tm_sec;
  return retVal;
}

namespace {
struct NumberWrapper {
  int val;
};

std::ostream &operator<<(std::ostream &stream, const NumberWrapper &subject) {
  if (subject.val < 10) {
    stream << '0' << subject.val;
  } else {
    stream << subject.val;
  }
  return stream;
}
} // namespace

std::ostream &operator<<(std::ostream &s, const TimeNow &subject) {
  s << NumberWrapper{subject.hours} << ':' << NumberWrapper{subject.minutes}
    << ':' << NumberWrapper{subject.seconds};
  return s;
}
} // namespace trd
