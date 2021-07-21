#ifndef ION_DEVICE_SHARED_DRIVERS_RESET_H
#define ION_DEVICE_SHARED_DRIVERS_RESET_H

#include <stdint.h>

namespace Ion {
namespace Device {
namespace Reset {

void core();
void jump(uint32_t jumpIsrVectorAddress, bool useMainStack = true);

}
}
}

#endif
