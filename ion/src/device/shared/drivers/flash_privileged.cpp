#include "flash_privileged.h"
#include <drivers/config/external_flash.h>
#include <drivers/config/internal_flash.h>
#include "external_flash_privileged.h"
#include "internal_flash_privileged.h"
#include <assert.h>

namespace Ion {
namespace Device {
namespace Flash {

void MassErase() {
  for (int i = InternalFlash::Config::NumberOfForbiddenFirstSectors; i < InternalFlash::Config::NumberOfSectors; i++) {
    // InternalFlash::MassErase is forbidden
    InternalFlash::EraseSector(i);
  }
  ExternalFlash::MassErase();
}

void EraseSector(int i) {
  assert(i >= 0 && i < TotalNumberOfSectors());
  if (SectorIsInInternalFlash(i)) {
    InternalFlash::EraseSector(i);
  } else {
    ExternalFlash::EraseSector(i - InternalFlash::Config::NumberOfSectors);
  }
}

void WriteMemory(uint8_t * destination, uint8_t * source, size_t length) {
  assert(SectorAtAddress((uint32_t)destination) >= 0);
  if (AddressIsInInternalFlash((uint32_t)destination)) {
    InternalFlash::WriteMemory(destination, source, length);
  } else {
    ExternalFlash::WriteMemory(destination - ExternalFlash::Config::StartAddress, source, length);
  }
}

}
}
}

