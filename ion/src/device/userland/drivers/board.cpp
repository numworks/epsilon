#include "board.h"
#include <userland/drivers/svcall.h>

namespace Ion {
namespace Device {
namespace Board {

void SVC_ATTRIBUTES switchExecutableSlot(uint32_t address) {
  SVC_RETURNING_VOID(SVC_BOARD_SWITCH_EXECUTABLE_SLOT);
}

void SVC_ATTRIBUTES downgradeTrustLevel(bool trustLevel) {
  SVC_RETURNING_VOID(SVC_BOARD_DOWNGRADE_TRUST_LEVEL);
}

}
}
}
