#include <ion.h>
#include <chrono>

static auto start = std::chrono::high_resolution_clock::now();

uint32_t Ion::millis() {
  auto elapsed = std::chrono::high_resolution_clock::now() - start;
  return std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();
}
