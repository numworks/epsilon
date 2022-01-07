#include "board.h"
#include <userland/drivers/svcall.h>

extern "C" {
extern char _external_apps_RAM_start;
extern char _external_apps_RAM_end;
}

namespace Ion {
namespace Device {
namespace Board {

void SVC_ATTRIBUTES switchExecutableSlot(uint32_t address) {
  SVC_RETURNING_VOID(SVC_BOARD_SWITCH_EXECUTABLE_SLOT);
}

void SVC_ATTRIBUTES enableExternalApps() {
  SVC_RETURNING_VOID(SVC_BOARD_ENABLE_EXTERNAL_APPS);
}

void appSandboxRange(uint32_t * start, uint32_t * end) {
  *start = reinterpret_cast<uint32_t>(&_external_apps_RAM_start);
  *end = reinterpret_cast<uint32_t>(&_external_apps_RAM_end);
}

}
}
}
