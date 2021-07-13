#include <drivers/board_unprivileged.h>
#include <drivers/config/board.h>
#include <drivers/kernel_header.h>

namespace Ion {
namespace Device {
namespace Board {

KernelHeader * kernelHeader() {
  return reinterpret_cast<KernelHeader *>((isRunningSlotA() ? Config::SlotAStartAddress : Config::SlotBStartAddress) +  Board::Config::SignedPayloadSize);
}

uint32_t slotAUserlandStart() {
  return Config::SlotAStartAddress + Config::UserlandOffset + sizeof(UserlandHeader);

}
uint32_t slotBUserlandStart() {
  return Config::SlotBStartAddress + Config::UserlandOffset+ sizeof(UserlandHeader);
}

}
}
}
