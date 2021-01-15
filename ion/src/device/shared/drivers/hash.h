#ifndef ION_DEVICE_SHARED_DRIVERS_HASH_H
#define ION_DEVICE_SHARED_DRIVERS_HASH_H

#include <stdint.h>
#include <stddef.h>

namespace Ion {
namespace Device {
namespace Hash {

constexpr size_t Sha256DigestBytes = 32;
void sha256(const uint8_t * data, uint32_t dataLength, uint8_t * digest);

}
}
}

#endif
