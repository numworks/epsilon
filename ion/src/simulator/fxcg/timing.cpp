#include <ion/timing.h>
#include "main.h"
#include <chrono>
// #include <libndls.h>

#include <gint/clock.h>

static auto start = std::chrono::steady_clock::now();

namespace Ion {
namespace Timing {

uint64_t millis() {
  auto elapsed = std::chrono::steady_clock::now() - start;
  return std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();
}

void usleep(uint32_t us) {
  sleep_us(us);
}

void msleep(uint32_t ms) {
  sleep_us(ms * 1000);
}

}
}