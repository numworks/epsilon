#include <shared/drivers/flash_unprivileged.h>
#include <drivers/config/internal_flash.h>
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
  return address >= InternalFlash::Config::StartAddress && address <= InternalFlash::Config::EndAddress;
}

}
}
}
