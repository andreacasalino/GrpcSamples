#include <Periodic.h>

namespace trd {
bool Periodic::poll(Spinner &caller) {
  auto now = std::chrono::high_resolution_clock::now();
  if (!last_shot.has_value() ||
      period < std::chrono::duration_cast<std::chrono::nanoseconds>(
                   now - last_shot.value())) {
    last_shot = now;
    return pred(caller);
  }
  return true;
}
} // namespace trd
