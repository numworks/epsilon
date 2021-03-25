#include "board.h"
#include <userland/drivers/svcall.h>

namespace Ion {
namespace Device {
namespace Board {

void switchExecutableSlotSVC(uint32_t * res) {
  SVC(SVC_BOARD_SWITCH_EXECUTABLE_SLOT);
}


uint32_t switchExecutableSlot() {
  uint32_t res;
  switchExecutableSlotSVC(&res);
  return res;
}

}
}
}
