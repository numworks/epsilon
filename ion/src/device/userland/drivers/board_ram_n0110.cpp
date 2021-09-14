#include <shared/drivers/config/board.h>

namespace Ion {
namespace Device {
namespace Board {

/* N0110 userland DFU cannot write the SRAM allocated for the kernel. */

uint32_t writableSRAMStartAdress() {
  return UserlandSRAMAddress;
}

uint32_t writableSRAMEndAdress() {
  return UserlandSRAMAddress + UserlandSRAMLength;
}

}
}
}

