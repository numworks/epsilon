#ifndef ION_DEVICE_SHARED_POWER_H
#define ION_DEVICE_SHARED_POWER_H

namespace Ion {
namespace Device {
namespace Power {

void internalFlashSuspend(bool isLEDActive = false);
void internalFlashStandby();
void configWakeUp();

void sleepConfiguration();
void stopConfiguration();

void waitUntilOnOffKeyReleased();
void enterLowPowerMode();

}
}
}

#endif
