#include <gtest/gtest.h>

#include <Periodic.h>
#include <Spinner.h>

#include <list>

using namespace fly;

using Ticks = std::vector<std::chrono::_V2::high_resolution_clock::time_point>;

struct PeriodicFixture : public Periodic {
  PeriodicFixture(const std::chrono::milliseconds &period, Ticks &ticks)
      : Periodic{period, [&](Spinner &caller) {
                   ticks.emplace_back(
                       std::chrono::high_resolution_clock::now());
                   return ++counter < 10;
                 }} {}

private:
  std::size_t counter = 0;
};

TEST(PeriodicTest, single) {
  Ticks ticks;

  Spinner spinner{
      std::make_unique<PeriodicFixture>(std::chrono::seconds{1}, ticks)};

  spinner.run();

  std::vector<std::chrono::milliseconds> deltas;
  for (std::size_t k = 1; k < ticks.size(); ++k) {
    deltas.emplace_back(std::chrono::duration_cast<std::chrono::milliseconds>(
        ticks[k] - ticks[k - 1]));
  }

  for (const auto &delta : deltas) {
    std::cout << delta.count() << std::endl;
  }

  std::cout << std::endl;
}
