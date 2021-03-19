#include "internal_flash_unprivileged.h"
#include <drivers/config/internal_flash.h>
#include <regs/regs.h>

namespace Ion {
namespace Device {
namespace InternalFlash {

int SectorAtAddress(uint32_t address) {
  for (int i = 0; i < Config::NumberOfSectors; i++) {
    if (address >= Config::SectorAddresses[i] && address < Config::SectorAddresses[i+1]) {
      return i;
    }
  }
  return -1;
}

}
}
}
