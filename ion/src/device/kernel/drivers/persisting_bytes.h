#ifndef ION_DEVICE_KERNEL_DRIVERS_PERSISTING_BYTES_H
#define ION_DEVICE_KERNEL_DRIVERS_PERSISTING_BYTES_H

#include <stdint.h>

namespace Ion {
namespace Device {
namespace PersistingBytes {

uint8_t read();
void write(uint8_t byte);

}
}
}

#endif

