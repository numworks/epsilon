#include "board.h"
#include <userland/drivers/svcall.h>

extern "C" {
extern uint8_t _app_sandbox_start;
extern uint8_t _app_sandbox_end;
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
  *start = reinterpret_cast<uint32_t>(&_app_sandbox_start);
  *end = reinterpret_cast<uint32_t>(&_app_sandbox_end);
}

}
}
}
