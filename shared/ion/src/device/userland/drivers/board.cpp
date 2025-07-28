#include "board.h"

#include "svcall.h"

namespace Ion {
namespace Device {
namespace Board {

void SVC_ATTRIBUTES
updateClearanceLevelForUnauthenticatedUserland(uint32_t address) {
  SVC_RETURNING_VOID(
      SVC_BOARD_UPDATE_CLEARANCE_LEVEL_FOR_UNAUTHENTICATED_USERLAND);
}

void SVC_ATTRIBUTES updateClearanceLevelForExternalApps() {
  SVC_RETURNING_VOID(SVC_BOARD_UPDATE_CLEARANCE_LEVEL_FOR_EXTERNAL_APPS);
}

}  // namespace Board
}  // namespace Device
}  // namespace Ion
