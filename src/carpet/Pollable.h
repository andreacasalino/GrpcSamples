#pragma once

#include <functional>

namespace carpet {
class Spinner;

class Pollable {
public:
  virtual ~Pollable() = default;

  virtual bool poll(Spinner &caller) = 0;
};

class PredicatePollable : public Pollable {
public:
  template<typename Pred>
  PredicatePollable(Pred&& p) : pred{std::forward<Pred>(p)} {}

  bool poll(Spinner &caller) final {return pred(caller);}

private:
  std::function<bool(Spinner &)> pred; 
};
} // namespace carpet
