#pragma once

#include <atomic>

namespace trd {
class SpinLockGuard {
public:
  SpinLockGuard(std::atomic_bool &lock) : lock{lock} {
    while (true) {
      bool expected = true;
      if (lock.compare_exchange_strong(expected, false,
                                       std::memory_order::acquire)) {
        break;
      }
    }
  }

  ~SpinLockGuard() { lock.store(true, std::memory_order::release); }

private:
  std::atomic_bool &lock;
};
} // namespace trd
