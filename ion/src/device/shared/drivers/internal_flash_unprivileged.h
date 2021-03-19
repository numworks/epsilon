#ifndef ION_DEVICE_SHARED_INTERNAL_FLASH_UNPRIVILEGED_H
#define ION_DEVICE_SHARED_INTERNAL_FLASH_UNPRIVILEGED_H

#include <stdint.h>

namespace Ion {
namespace Device {
namespace InternalFlash {

int SectorAtAddress(uint32_t address);

}
}
}

#endif
