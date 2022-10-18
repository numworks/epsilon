#ifndef ION_DEVICE_CORE_SHARED_FLASH_PRIVILEGED_H
#define ION_DEVICE_CORE_SHARED_FLASH_PRIVILEGED_H

#include <stddef.h>
#include <stdint.h>

namespace Ion {
namespace Device {
namespace Flash {

bool WriteMemory(uint8_t * destination, const uint8_t * source, size_t length);

}
}
}

#endif
