#include <config/board.h>
#include <drivers/ram_layout.h>

namespace Ion {
namespace Device {
namespace Board {

/* N0110 userland DFU cannot write the SRAM allocated for the kernel. */

uint32_t writableSRAMStartAddress() {
  return UserlandSRAMAddress;
}

uint32_t writableSRAMEndAddress() {
  return UserlandSRAMAddress + UserlandSRAMLength;
}

}
}
}

