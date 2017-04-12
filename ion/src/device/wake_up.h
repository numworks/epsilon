#ifndef ION_DEVICE_WAKE_UP_H
#define ION_DEVICE_WAKE_UP_H

#include "regs/regs.h"

namespace Ion {
namespace WakeUp {
namespace Device {

void onChargingEvent();
void onUSBPlugging();
void onPowerKeyDown();

}
}
}

#endif
