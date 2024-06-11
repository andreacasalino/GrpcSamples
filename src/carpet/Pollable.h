#pragma once

namespace trd {
class Spinner;

class Pollable {
public:
  virtual ~Pollable() = default;

  virtual bool poll(Spinner &caller) = 0;
};
} // namespace trd
