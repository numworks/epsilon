#ifndef ION_DEVICE_USERLAND_DRIVERS_PERSISTING_BYTES_H
#define ION_DEVICE_USERLAND_DRIVERS_PERSISTING_BYTES_H

#include <stdint.h>

namespace Ion {
namespace PersistingBytes {

uint8_t* bufferStart();
uint8_t* bufferEnd();

}  // namespace PersistingBytes
}  // namespace Ion

#endif
