#ifndef ION_DEVICE_SHARED_TIMING_H
#define ION_DEVICE_SHARED_TIMING_H

#include <stdint.h>

namespace Ion {
namespace Device {
namespace Timing {

void init();
void shutdown();

extern volatile uint64_t MillisElapsed;

}
}
}

#endif
