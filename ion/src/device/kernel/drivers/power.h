#ifndef ION_DEVICE_KERNEL_DRIVERS_POWER_H
#define ION_DEVICE_KERNEL_DRIVERS_POWER_H

namespace Ion {
namespace Device {
namespace Power {

void selectStandbyMode(bool standbyMode);
void suspend(bool checkIfOnOffKeyReleased);
void standby();

void waitUntilOnOffKeyReleased();
void configWakeUp();
void sleepConfiguration();
void stopConfiguration();
void bootloaderSuspend();

}
}
}

#endif

