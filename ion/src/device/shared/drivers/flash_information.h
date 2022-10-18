#ifndef ION_DEVICE_SHARED_FLASH_INFORMATION_H
#define ION_DEVICE_SHARED_FLASH_INFORMATION_H

#include <stdint.h>

namespace Ion {
namespace Device {
namespace Flash {

int TotalNumberOfSectors();
int SectorAtAddress(uint32_t address);
bool IncludesAddress(uint32_t address);

}
}
}

#endif
