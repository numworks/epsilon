#ifndef ION_DEVICE_SHARED_POWER_H
#define ION_DEVICE_SHARED_POWER_H

namespace Ion {
namespace Device {
namespace Power {

void internal_flash_suspend(bool isLEDActive = false);
void internal_flash_standby();
void configWakeUp();

void sleepConfiguration();
void stopConfiguration();
void standbyConfiguration();

void waitUntilOnOffKeyReleased();
void enterLowPowerMode();

}
}
}

#endif
