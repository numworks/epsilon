#include <config/internal_flash.h>
#include <shared/drivers/flash_privileged.h>
#include <shared/drivers/flash_unprivileged.h>
#include <shared/drivers/internal_flash.h>
#include <regs/regs.h>

namespace Ion {
namespace Device {
namespace Flash {

int TotalNumberOfSectors() {
  return InternalFlash::Config::NumberOfSectors;
}

int SectorAtAddress(uint32_t address) {
  for (int i = 0; i < InternalFlash::Config::NumberOfSectors; i++) {
    if (address >= InternalFlash::Config::SectorAddresses[i] && address < InternalFlash::Config::SectorAddresses[i+1]) {
      return i;
    }
  }
  return -1;
}

bool IncludesAddress(uint32_t address) {
  return address >= Config::InternalFlashOrigin && address <= Config::InternalFlashOrigin + Config::InternalFlashLength;
}

using namespace Regs;

bool ForbiddenSector(int i) {
  return i < 0 || i > InternalFlash::Config::NumberOfSectors;
}

bool MassEraseEnable() {
  return true;
}

void MassErase() {
  if (MassEraseEnable()) {
    InternalFlash::MassErase();
  }
}

bool EraseSector(int i) {
  if (ForbiddenSector(i)) {
    return false;
  }
  InternalFlash::EraseSector(i);
  return true;
}

bool WriteMemory(uint8_t * destination, const uint8_t * source, size_t length) {
  uint32_t address = reinterpret_cast<uint32_t>(destination);
  if (Config::InternalFlashOrigin <= address && address < Config::InternalFlashOrigin + Config::InternalFlashLength) {
    InternalFlash::WriteMemory(destination, source, length);
    return true;
  }
  return false;
}

}
}
}
