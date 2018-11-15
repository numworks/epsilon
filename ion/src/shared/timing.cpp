#include <ion.h>
#include <chrono>

static auto start = std::chrono::steady_clock::now();

uint64_t Ion::Timing::millis() {
  auto elapsed = std::chrono::steady_clock::now() - start;
  return std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();
}
