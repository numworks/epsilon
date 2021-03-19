#ifndef ION_DEVICE_SHARED_DRIVERS_FLASH_UNPRIVILEGED_H
#define ION_DEVICE_SHARED_DRIVERS_FLASH_UNPRIVILEGED_H

#include <stdint.h>

namespace Ion {
namespace Device {
namespace Flash {

bool AddressIsInInternalFlash(uint32_t address);
bool AddressIsInExternalFlash(uint32_t address);
int TotalNumberOfSectors();
int SectorAtAddress(uint32_t address);
bool SectorIsInInternalFlash(int i);
bool SectorIsInExternalFlash(int i);
bool SectorIsWritableViaDFU(int i);

}
}
}

#endif
