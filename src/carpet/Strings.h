#pragma once

#include <ostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>

namespace carpet {
struct Strings {
  template <char Sep, typename First, typename... ARGS>
  static std::string join(First &&first, ARGS &&...others) {
    std::stringstream tmp;
    joinStream<Sep>(tmp, std::forward<First>(first),
                    std::forward<ARGS>(others)...);
    return tmp.str();
  }

  template <char Sep, typename First, typename... ARGS>
  static void joinStream(std::ostream &recipient, First &&first,
                         ARGS &&...others) {
    join_<Sep>(recipient, std::forward<First>(first),
               std::forward<ARGS>(others)...);
  }

  template <char Separator, typename StringT>
  static std::pair<std::string_view, std::string_view> trim(StringT &subject) {
    std::size_t pos = subject.find(Separator);
    if (pos == std::string::npos) {
      throw std::runtime_error{Strings::join<' '>("unable to find ", Separator,
                                                  " inside ", subject)};
    }
    std::string_view first{subject.data(), pos};
    std::string_view second{subject.data() + pos + 1,
                            subject.size() - 1 - first.size()};
    return std::make_pair(first, second);
  }

  template <char Separator, typename StringT, typename Pred>
  static void forEachFragment(StringT &subject, Pred process) {
    std::size_t cursor = 0;
    while (cursor < subject.size()) {
      auto next = subject.find(Separator, cursor);
      if (next == std::string::npos) {
        process(
            std::string_view{subject.data() + cursor, subject.size() - cursor});
        break;
      }
      process(std::string_view{subject.data() + cursor, next - cursor});
      cursor = next + 1;
    }
  }

private:
  template <char Sep, typename First, typename... ARGS>
  static void join_(std::ostream &recipient, First &&first, ARGS &&...args) {
    recipient << first << Sep;
    join_<Sep>(recipient, std::forward<ARGS>(args)...);
  }

  template <char Sep, typename T>
  static void join_(std::ostream &recipient, T &&el) {
    recipient << el;
  }
};
} // namespace carpet
