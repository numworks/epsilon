#ifndef ION_DEVICE_KERNEL_DRIVERS_AUTHENTICATION_H
#define ION_DEVICE_KERNEL_DRIVERS_AUTHENTICATION_H

#include <ion/authentication.h>
#include <stdint.h>

namespace Ion {
namespace Device {
namespace Authentication {

Ion::Authentication::ClearanceLevel clearanceLevel();
void downgradeClearanceLevelTo(Ion::Authentication::ClearanceLevel level);

}
}
}

#endif
