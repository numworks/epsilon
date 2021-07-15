#include <boot/rt0.h>
#include <kernel/drivers/board.h>

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

