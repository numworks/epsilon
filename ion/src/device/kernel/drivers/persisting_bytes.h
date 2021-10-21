#ifndef ION_DEVICE_KERNEL_DRIVERS_PERSISTING_BYTES_H
#define ION_DEVICE_KERNEL_DRIVERS_PERSISTING_BYTES_H

#include <stdint.h>
#include <ion/persisting_bytes.h>

namespace Ion {
namespace Device {
namespace PersistingBytes {

uint8_t read(uint8_t index);
void write(uint8_t byte, uint8_t index);
static constexpr uint8_t k_numberOfPersistingBytes = Ion::PersistingBytes::k_numberOfPersistingBytes;

}
}
}

#endif

