#pragma once

#include <Strings.h>

#include <stdexcept>

namespace carpet {
class Error : public std::runtime_error {
public:
  template <typename... ARGS>
  Error(std::string file, int line, ARGS &&...args)
      : std::runtime_error{Strings::join<' '>(make_preamble(file, line),
                                              std::forward<ARGS>(args)...)} {}

private:
  static std::string make_preamble(std::string file, int line);
};
} // namespace carpet

#define THROW_ERROR(...) throw trd::Error{__FILE__, __LINE__, __VA_ARGS__};
