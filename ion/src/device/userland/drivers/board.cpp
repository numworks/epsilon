#include "board.h"

#include <drivers/svcall.h>

namespace Ion {
namespace Device {
namespace Board {

void SVC_ATTRIBUTES switchExecutableSlot(uint32_t address) {
  SVC_RETURNING_VOID(SVC_BOARD_SWITCH_EXECUTABLE_SLOT);
}

void SVC_ATTRIBUTES updateClearanceLevelForExternalApps() {
  SVC_RETURNING_VOID(SVC_BOARD_UPDATE_CLEARANCE_LEVEL_FOR_EXTERNAL_APPS);
}

}  // namespace Board
}  // namespace Device
}  // namespace Ion
