#ifndef ION_DEVICE_KERNEL_DRIVERS_POWER_H
#define ION_DEVICE_KERNEL_DRIVERS_POWER_H

#include <shared/drivers/power.h>

namespace Ion {
namespace Device {
namespace Power {

void selectStandbyMode(bool standbyMode);
void suspend(bool checkIfOnOffKeyReleased);

void configWakeUp();

}
}
}

#endif

