#pragma once

namespace carpet {
class Spinner;

class Pollable {
public:
  virtual ~Pollable() = default;

  virtual bool poll(Spinner &caller) = 0;
};
} // namespace carpet
