#pragma once

#include <Periodic.h>
#include <Spinner.h>

#include <chrono>
#include <functional>
#include <optional>

namespace carpet {
class Periodic : public Pollable {
public:
  template <typename PeriodT, typename Pred>
  Periodic(const PeriodT &t, Pred p)
      : period{std::chrono::duration_cast<std::chrono::nanoseconds>(t)},
        pred{std::forward<Pred>(p)} {}

  bool poll(Spinner &caller) final;

private:
  using TimePoint = std::chrono::_V2::high_resolution_clock::time_point;
  std::optional<TimePoint> last_shot;
  std::chrono::nanoseconds period;
  std::function<bool(Spinner &)> pred;
};
} // namespace carpet
