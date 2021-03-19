#include "board.h"
#include <userland/drivers/svcall.h>

namespace Ion {
namespace Device {
namespace Board {

void SVC_ATTRIBUTES switchExecutableSlotSVC(int * deltaKernelVersion, int * deltaUserlandVersion) {
  SVC(SVC_BOARD_SWITCH_EXECUTABLE_SLOT);
}
void switchExecutableSlot(int deltaKernelVersion, int deltaUserlandVersion) {
  switchExecutableSlotSVC(&deltaKernelVersion, &deltaUserlandVersion);
}

}
}
}
