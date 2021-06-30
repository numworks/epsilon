#include <shared/drivers/flash_privileged.h>
#include <shared/drivers/config/external_flash.h>
#include <shared/drivers/config/internal_flash.h>
#include <shared/drivers/external_flash.h>
#include <shared/drivers/internal_flash.h>

namespace Ion {
namespace Device {
namespace Flash {

bool ForbiddenSector(int i) {
  return false;
}

bool MassEraseEnable() {
  return true;
}

void MassErase() {
  assert(MassEraseEnable());
  InternalFlash::MassErase();
  ExternalFlash::MassErase();
}

bool EraseSector(int i) {
  assert(!ForbiddenSector(i));
  if (i < 0) {
    return false;
  }
  if (i < InternalFlash::Config::NumberOfSectors) {
    InternalFlash::EraseSector(i);
    return true;
  }
  i -= InternalFlash::Config::NumberOfSectors;
  if (i < ExternalFlash::Config::NumberOfSectors) {
    ExternalFlash::EraseSector(i);
    return true;
  }
  return false;
}

bool WriteMemory(uint8_t * destination, const uint8_t * source, size_t length) {
  uint32_t address = reinterpret_cast<uint32_t>(destination);
  if (InternalFlash::Config::StartAddress <= address && address < InternalFlash::Config::EndAddress) {
    InternalFlash::WriteMemory(destination, source, length);
    return true;
  } else if (ExternalFlash::Config::StartAddress <= address && address < ExternalFlash::Config::EndAddress) {
    ExternalFlash::WriteMemory(destination, source, length);
    return true;
  }
  return false;
}

}
}
}
