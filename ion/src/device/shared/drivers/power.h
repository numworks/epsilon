#ifndef ION_DEVICE_SHARED_DRIVERS_POWER_SUSPEND_H
#define ION_DEVICE_SHARED_DRIVERS_POWER_SUSPEND_H

namespace Ion {
namespace Device {
namespace Power {

void standby();
void standbyConfiguration();
void sleepConfiguration();
void stopConfiguration();
void bootloaderSuspend();

void waitUntilOnOffKeyReleased();

}
}
}

#endif
