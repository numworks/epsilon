#ifndef ION_DEVICE_KERNEL_DRIVERS_AUTHENTICATION_H
#define ION_DEVICE_KERNEL_DRIVERS_AUTHENTICATION_H

#include <stdint.h>

namespace Ion {
namespace Device {
namespace Authentication {

bool trustedUserland();
bool updateTrust();

}
}
}

#endif
