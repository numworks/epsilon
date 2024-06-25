#include <config/board.h>
#include <shared/drivers/board_shared.h>
#include <shared/drivers/kernel_header.h>
#include <shared/drivers/userland_header.h>

namespace Ion {
namespace Device {
namespace Board {

KernelHeader* kernelHeader() {
  uint32_t slotOffset =
      isRunningSlotA() ? Config::SlotAOffset : Config::SlotBOffset;
  return reinterpret_cast<KernelHeader*>(
      Config::KernelVirtualOrigin + slotOffset + Config::SignedPayloadLength);
}

UserlandHeader* userlandHeader() {
  return userlandHeader(isRunningSlotA() ? Slot::A : Slot::B);
}

UserlandHeader* userlandHeader(Slot slot) {
  uint32_t slotOffset =
      slot == Slot::A ? Config::SlotAOffset : Config::SlotBOffset;
  return reinterpret_cast<UserlandHeader*>(Config::UserlandVirtualOrigin +
                                           slotOffset);
}

uint32_t userlandStart() {
  return userlandStart(isRunningSlotA() ? Slot::A : Slot::B);
}

uint32_t userlandStart(Slot slot) {
  return reinterpret_cast<uint32_t>(userlandHeader(slot)) +
         sizeof(UserlandHeader);
}

uint32_t userlandEnd(Slot slot) {
  return reinterpret_cast<uint32_t>(userlandHeader(slot)) +
         Config::UserlandLength;
}

uint32_t securityLevel() {
  return *(reinterpret_cast<uint32_t*>(isRunningSlotA() ? Config::SlotAOrigin
                                                        : Config::SlotBOrigin));
}

}  // namespace Board
}  // namespace Device
}  // namespace Ion
