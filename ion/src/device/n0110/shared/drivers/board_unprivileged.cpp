#include <drivers/board_unprivileged.h>
#include <drivers/config/board.h>
#include <drivers/kernel_header.h>

namespace Ion {
namespace Device {
namespace Board {

KernelHeader * kernelHeader() {
  return reinterpret_cast<KernelHeader *>((isRunningSlotA() ? Config::SlotAStartAddress : Config::SlotBStartAddress) +  Board::Config::SignedPayloadSize);
}

UserlandHeader * userlandHeader() {
  uint32_t slotStart = isRunningSlotA() ?  Config::SlotAStartAddress : Config::SlotBStartAddress;
  return reinterpret_cast<UserlandHeader *>(slotStart + Config::UserlandOffset);
}

}
}
}
