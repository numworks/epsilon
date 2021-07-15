#include <ion/src/device/bench/bench.h>
#include <ion/backlight.h>

void ion_main(int argc, const char * const argv[]) {
  // Initialize the backlight
  Ion::Backlight::init();
  Ion::Device::Bench::run();
}
