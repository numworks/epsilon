#ifndef ION_DEVICE_RTC_H
#define ION_DEVICE_RTC_H

#include "regs/regs.h"

namespace Ion {
namespace Device {
namespace RTC {

void init(bool enabled, bool useHighPrecisionClock);

}
}
}

#endif
