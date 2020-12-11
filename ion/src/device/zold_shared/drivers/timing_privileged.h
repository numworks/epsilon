#ifndef ION_DEVICE_SHARED_TIMING_PRIVILEGED_H
#define ION_DEVICE_SHARED_TIMING_PRIVILEGED_H

#include <stdint.h>

namespace Ion {
namespace Device {
namespace Timing {

void init();
void shutdown();
void setSysTickFrequency(int frequencyInMHz);

}
}
}

#endif
