#ifndef ION_DEVICE_SHARED_DRIVERS_TIMING_H
#define ION_DEVICE_SHARED_DRIVERS_TIMING_H

#include <stdint.h>

namespace Ion {
namespace Device {
namespace Timing {

void usleep(uint32_t us);
void msleep(uint32_t ms);

}
}
}

#endif
