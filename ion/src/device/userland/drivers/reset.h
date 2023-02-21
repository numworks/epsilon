#ifndef ION_DEVICE_USERLAND_DRIVERS_RESET_H
#define ION_DEVICE_USERLAND_DRIVERS_RESET_H

#include <stdint.h>

namespace Ion {
namespace Device {
namespace Reset {

void jump(uint32_t jumpIsrVectorAddress);

}
}  // namespace Device
}  // namespace Ion

#endif
