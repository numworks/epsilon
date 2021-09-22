#include <config/board.h>
#include <drivers/board.h>
#include <drivers/kernel_header.h>
#include <drivers/ram_layout.h>

namespace Ion {
namespace Device {
namespace Board {

KernelHeader * kernelHeader() {
  return reinterpret_cast<KernelHeader *>((isRunningSlotA() ? Config::SlotAStartAddress : Config::SlotBStartAddress) +  Board::SignedPayloadSize);
}

UserlandHeader * userlandHeader() {
  uint32_t slotStart = isRunningSlotA() ?  Config::SlotAStartAddress : Config::SlotBStartAddress;
  return reinterpret_cast<UserlandHeader *>(slotStart + Config::UserlandOffset);
}

}
}
}
