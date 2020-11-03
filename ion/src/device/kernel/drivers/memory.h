#ifndef ION_DEVICE_KERNEL_DRIVERS_MEMORY_H
#define ION_DEVICE_KERNEL_DRIVERS_MEMORY_H

#include <stdint.h>

namespace Ion {
namespace Device {
namespace Memory {

void PersistByte(uint8_t byte);
uint8_t PersistedByte();

}
}
}

#endif

