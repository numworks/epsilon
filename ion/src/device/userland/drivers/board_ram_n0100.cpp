#include <shared/drivers/config/board.h>

namespace Ion {
namespace Device {
namespace Board {

/* N0100 userland DFU can't write:
 * - the SRAM allocated for the kernel
 * - the Userland stack where the DFU driver is actually living (it cannot
 *   override itself) */

uint32_t writableSRAMStartAdress() {
  return UserlandSRAMAddress;
}

uint32_t writableSRAMEndAdress() {
  return UserlandSRAMAddress + UserlandSRAMLength - UserlandStackLength;;
}

}
}
}

