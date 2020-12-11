#ifndef ION_DEVICE_SHARED_POWER_PRIVILEGED_H
#define ION_DEVICE_SHARED_POWER_PRIVILEGED_H

#include "power.h"

namespace Ion {
namespace Device {
namespace Power {

void sleepStopHandler();
void standbyHandler();

void internalFlashSuspend(bool isLEDActive = false);
void internalFlashStandby();
void configWakeUp();
void sleepConfiguration();
void stopConfiguration();
void enterLowPowerMode();

}
}
}

#endif
