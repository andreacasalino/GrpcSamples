#include <Spinner.h>

#include <thread>

namespace carpet {
void Spinner::run(const std::chrono::milliseconds &poll_time) {
  while (!pollables.empty()) {
    auto tic = std::chrono::high_resolution_clock::now();

    auto it = pollables.begin();
    while (it != pollables.end()) {
      if ((*it)->poll(*this)) {
        ++it;
      } else {
        it = pollables.erase(it);
      }
    }
    for (auto &to_add : new_pollables_queue) {
      pollables.emplace_back(std::move(to_add));
    }
    new_pollables_queue.clear();

    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::high_resolution_clock::now() - tic);
    if (elapsed < poll_time) {
      std::this_thread::sleep_for(poll_time - elapsed);
    }
  }
}
} // namespace carpet
