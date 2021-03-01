#ifndef ION_DEVICE_SHARED_DRIVERS_AUTHENTICATION_H
#define ION_DEVICE_SHARED_DRIVERS_AUTHENTICATION_H

#include <stddef.h>
#include <stdint.h>

namespace Ion {
namespace Device {
namespace Authentication {

bool verify(uint8_t * signature, uint8_t * code, uint32_t codeSize);

}
}
}

#endif
