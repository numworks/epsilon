#include <config/board.h>
#include <shared/drivers/board_shared.h>
#include <shared/drivers/kernel_header.h>
#include <shared/drivers/userland_header.h>


extern "C" {
  extern char _persisting_bytes_buffer_start;
}

namespace Ion {
namespace Device {
namespace Board {

KernelHeader * kernelHeader() {
  uint32_t slotOffset = isRunningSlotA() ? Config::SlotAOffset : Config::SlotBOffset;
  return reinterpret_cast<KernelHeader *>(Config::KernelVirtualOrigin + slotOffset + Config::SignedPayloadLength);
}

UserlandHeader * userlandHeader() {
  uint32_t slotOffset = isRunningSlotA() ? Config::SlotAOffset : Config::SlotBOffset;
  return reinterpret_cast<UserlandHeader *>(Config::UserlandVirtualOrigin + slotOffset);
}

uint32_t userlandStart() {
  return reinterpret_cast<uint32_t>(userlandHeader()) + sizeof(UserlandHeader);
}

uint32_t userlandEnd() {
  // On running slot, userland range is USERLAND + SIGNATURE + EXTERN APPS
  // || SP* HEADER | KERNEL | USERLAND | SIGNATURE | EXTERN APPS | PERSISTING BYTES
  return reinterpret_cast<uint32_t>(&_persisting_bytes_buffer_start);
}

uint32_t securityLevel() {
  return *(reinterpret_cast<uint32_t *>(isRunningSlotA() ? Config::SlotAOrigin : Config::SlotBOrigin));
}

}
}
}
