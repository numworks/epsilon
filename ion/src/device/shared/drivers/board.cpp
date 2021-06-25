#include <shared/drivers/board.h>
#include <drivers/userland_header.h>

namespace Ion {
namespace Device {
namespace Board {

uint32_t userlandStart() {
  return isRunningSlotA() ? slotAUserlandStart() : slotBUserlandStart();
}

UserlandHeader * userlandHeader() {
  return reinterpret_cast<UserlandHeader *>(userlandStart() - sizeof(UserlandHeader));
}

uint32_t otherUserlandStart() {
  return isRunningSlotA() ? slotBUserlandStart() : slotAUserlandStart();
}

UserlandHeader * otherUserlandHeader() {
  return reinterpret_cast<UserlandHeader *>(otherUserlandStart() - sizeof(UserlandHeader));
}

}
}
}
