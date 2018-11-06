#include <stdint.h>
#include <ion.h>

void Ion::msleep(long ms) {
}

#include <chrono>

static auto start = std::chrono::high_resolution_clock::now();

uint32_t Ion::millis() {
  auto elapsed = std::chrono::high_resolution_clock::now() - start;
  return std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();
}

uint32_t Ion::micros() {
  auto elapsed = std::chrono::high_resolution_clock::now() - start;
  return std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
}
