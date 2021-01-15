#ifndef ION_DEVICE_SHARED_DRIVERS_AUTHENTICATION_H
#define ION_DEVICE_SHARED_DRIVERS_AUTHENTICATION_H

#include <stdint.h>

namespace Ion {
namespace Device {
namespace Authentication {

void decrypt(uint8_t * signature, uint8_t * decryptedSignature);

}
}
}

#endif
