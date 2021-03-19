#ifndef ION_DEVICE_KERNEL_DRIVERS_TIMING_H
#define ION_DEVICE_KERNEL_DRIVERS_TIMING_H

#include <stdint.h>
#include <ion/timing.h>

namespace Ion {
namespace Timing {

uint64_t millis();

}
}

namespace Ion {
namespace Device {
namespace Timing {

void init();
void shutdown();
void initInterruptions();
void shutdownInterruptions();
void setSysTickFrequency(int frequencyInMHz);

extern volatile uint64_t MillisElapsed;

}
}
}

#endif
