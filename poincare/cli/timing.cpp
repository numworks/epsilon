#include <ion/timing.h>

#include <chrono>

static auto start = std::chrono::steady_clock::now();

namespace Ion {
namespace Timing {

uint64_t millis() {
  auto elapsed = std::chrono::steady_clock::now() - start;
  return std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();
}

void msleep(uint32_t ms) {}

}  // namespace Timing
}  // namespace Ion
