#ifndef ION_DEVICE_SHARED_FLASH_WRITE_H
#define ION_DEVICE_SHARED_FLASH_WRITE_H

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
