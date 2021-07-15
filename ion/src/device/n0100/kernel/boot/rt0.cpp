#include <boot/rt0.h>
#include <kernel/drivers/display.h>
#include <kernel/drivers/display_image.h>
#include <kernel/drivers/board.h>
#include <kernel/drivers/timing.h>
#include <main.h>

namespace Ion {
namespace Device {
namespace Init {

void prologue() {
  /* Initialize the FPU as early as possible.
   * For example, static C++ objects are very likely to manipulate float values */
  Ion::Device::Board::initFPU();
}

}
}
}

void __attribute__((noinline)) jump_to_main() {
  Ion::Device::Board::initPeripherals(false);
  Ion::Device::DisplayImage::logo();
  Ion::Device::Display::waitForVBlank();
  Ion::Device::Timing::msleep(4000);
  return kernel_main();
}
