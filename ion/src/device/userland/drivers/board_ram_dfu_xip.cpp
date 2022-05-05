#include <config/board.h>

namespace Ion {
namespace Device {
namespace Board {

/* N0110 userland DFU cannot write the SRAM allocated for the kernel. */

uint32_t writableSRAMStartAddress() {
  return Device::Config::UserlandSRAMOrigin;
}

uint32_t writableSRAMEndAddress() {
  return Device::Config::UserlandSRAMOrigin + Device::Config::UserlandSRAMLength;
}

}
}
}

