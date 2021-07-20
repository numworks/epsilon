#include "external_flash_write.h"
#include <drivers/config/external_flash.h>
#include <shared/drivers/flash_privileged.h>

namespace Ion {
namespace Device {
namespace Flash {

void MassErase() {
  if (ExternalFlash::Config::NumberOfSectors == 0) {
    return;
  }
  if (MassEraseEnable()) {
    ExternalFlash::MassErase();
  }
  for (int i = 0; i <= TotalNumberOfSectors(); i++) {
    if (!ForbiddenSector(i)) {
      ExternalFlash::EraseSector(i);
    }
  }
}

bool EraseSector(int i) {
  assert(i >= 0 && i < ExternalFlash::Config::NumberOfSectors);
  if (ForbiddenSector(i)) {
    return false;
  }
  ExternalFlash::EraseSector(i);
  return true;
}

bool WriteMemory(uint8_t * destination, const uint8_t * source, size_t length) {
  if (ExternalFlash::Config::NumberOfSectors == 0) {
    return false;
  }
  int firstSector = SectorAtAddress(reinterpret_cast<uint32_t >(destination));
  int lastSector = SectorAtAddress(reinterpret_cast<uint32_t >(destination + length - 1));
  for (int i = firstSector; i <= lastSector; i++) {
    if (ForbiddenSector(i)) {
      return false;
    }
  }
  ExternalFlash::WriteMemory(destination, source, length);
  return true;
}

}
}
}
