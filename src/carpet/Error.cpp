#include <Error.h>

namespace carpet {
std::string Error::make_preamble(std::string file, int line) {
  std::string res = file;
  res += ':';
  res += std::to_string(line);
  return res;
}
} // namespace carpet
