#include <config/board.h>

namespace Ion {
namespace Device {
namespace Board {

/* Userland DFU cannot write the SRAM allocated for the kernel or its own
 * process stack. */

uint32_t writableSRAMStartAddress() {
  return Config::UserlandSRAMOrigin;
}

uint32_t writableSRAMEndAddress() {
  return Config::UserlandSRAMOrigin + Config::UserlandSRAMLength - Config::UserlandStackLength;
}

}
}
}
