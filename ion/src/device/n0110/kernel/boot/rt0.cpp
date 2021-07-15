#include <boot/rt0.h>
#include <kernel/drivers/board.h>
#include <main.h>

namespace Ion {
namespace Device {
namespace Init {

void prologue() {}

}
}
}

void __attribute__((noinline)) jump_to_main() {
  Ion::Device::Board::initPeripherals(true);
  return kernel_main();
}
