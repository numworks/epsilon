#ifndef ION_DEVICE_USERLAND_DRIVERS_BOARD_H
#define ION_DEVICE_USERLAND_DRIVERS_BOARD_H

#include <stdint.h>
#include <shared/drivers/board_unprivileged.h>

namespace Ion {
namespace Device {
namespace Board {

void switchExecutableSlot(uint32_t leaveAddress);
void enableExternalApps();
void appSandboxRange(uint32_t * start, uint32_t * end);

}
}
}

#endif

