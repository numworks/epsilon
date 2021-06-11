#include <shared/drivers/board.h>

namespace Ion {
namespace Device {
namespace Board {

uint32_t userlandStart() {
  return isRunningSlotA() ? slotAUserlandStart() : slotBUserlandStart();
}

uint32_t otherUserlandStart() {
  return isRunningSlotA() ? slotBUserlandStart() : slotAUserlandStart();
}

}
}
}
