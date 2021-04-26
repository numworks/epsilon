#include "svc.h"

constexpr const char title[] = "Third-party app";
constexpr const char upperTitle[] = "THIRD-PARTY APP";

void app_main() {
  // Configure data/bss in RAM?
  // Initialize data?
  Ion::Display::pushRectUniform(Rect(0,20,100,100), Color::RGB24(0xFF00FF));
  while (1) {}
}
