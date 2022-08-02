#ifndef ION_DEVICE_USERLAND_DRIVERS_BOARD_H
#define ION_DEVICE_USERLAND_DRIVERS_BOARD_H

#include <stdint.h>

namespace Ion {
namespace Device {
namespace Board {

void switchExecutableSlot(uint32_t leaveAddress);
void enableExternalApps();

}
}
}

#endif

