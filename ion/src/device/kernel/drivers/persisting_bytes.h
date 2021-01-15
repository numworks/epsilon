#ifndef ION_DEVICE_KERNEL_DRIVERS_PERSISTING_BYTES_H
#define ION_DEVICE_KERNEL_DRIVERS_PERSISTING_BYTES_H

#include <stdint.h>

namespace Ion {
namespace Device {
namespace PersistingBytes {

/*
 * TODO EMILIE:
 * - Optimize? don't eat another byte if you can avoid
 * - Robustize? Do we look for the first non 0xFF backwards? Or are we subject to corrupted memory? */
uint8_t read();
void write(uint8_t byte);

}
}
}

#endif

