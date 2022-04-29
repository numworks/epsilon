#include <config/board.h>
#include <drivers/board.h>
#include <drivers/kernel_header.h>
#include <drivers/ram_layout.h>

extern "C" {
  extern char _persisting_bytes_buffer_start;
}

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

uint32_t userlandEnd() {
  // On running slot, userland range is USERLAND + SIGNATURE + EXTERN APPS
  // || SP* HEADER | KERNEL | USERLAND | SIGNATURE | EXTERN APPS | PERSISTING BYTES
  return reinterpret_cast<uint32_t>(&_persisting_bytes_buffer_start);
}

}
}
}
