#pragma once

#include <Pollable.h>

#include <chrono>
#include <list>
#include <memory>

namespace trd {
using PollablePtr = std::unique_ptr<Pollable>;

class Spinner {
public:
  template <typename... Pollables>
  Spinner(PollablePtr &&entry_point, Pollables &&...args) {
    addInitialPollable(std::move(entry_point));
    (addInitialPollable(std::forward<Pollables>(args)), ...);
  }

  void run(const std::chrono::milliseconds &poll_time =
               std::chrono::milliseconds{50});

  void addPollable(PollablePtr pollable) {
    new_pollables_queue.emplace_back(std::move(pollable));
  }

private:
  void addInitialPollable(PollablePtr &&pollable) {
    pollables.emplace_back(std::move(pollable));
  }

  std::list<PollablePtr> pollables;
  std::list<PollablePtr> new_pollables_queue;
};
} // namespace trd
