#include "board.h"
#include <userland/drivers/svcall.h>

namespace Ion {
namespace Device {
namespace Board {

uint32_t SVC_ATTRIBUTES switchExecutableSlot() {
  SVC_RETURNING_R0(SVC_BOARD_SWITCH_EXECUTABLE_SLOT, uint32_t);
}

void SVC_ATTRIBUTES downgradeTrustLevel(bool trustLevel) {
  SVC_RETURNING_VOID(SVC_BOARD_DOWNGRADE_TRUST_LEVEL);
}

}
}
}
