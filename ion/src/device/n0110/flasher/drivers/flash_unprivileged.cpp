#include <shared/drivers/flash_unprivileged.h>
#include <drivers/config/external_flash.h>
#include <drivers/config/internal_flash.h>
#include <assert.h>

namespace Ion {
namespace Device {
namespace Flash {

int TotalNumberOfSectors() {
  return InternalFlash::Config::NumberOfSectors + ExternalFlash::Config::NumberOfSectors;
}

static int internalFlashSectorAtAddress(uint32_t address) {
  for (int i = 0; i < InternalFlash::Config::NumberOfSectors; i++) {
    if (address >= InternalFlash::Config::SectorAddresses[i] && address < InternalFlash::Config::SectorAddresses[i+1]) {
      return i;
    }
  }
  return -1;
}

static int externalFlashSectorAtAddress(uint32_t address) {
  /* WARNING: this code assumes that the flash sectors are of increasing size:
   * first all 4K sectors, then all 32K sectors, and finally all 64K sectors. */
  int i = address >> ExternalFlash::Config::NumberOfAddressBitsIn64KbyteBlock;
  if (i > ExternalFlash::Config::NumberOf64KSectors) {
    return -1;
  }
  if (i >= 1) {
    return ExternalFlash::Config::NumberOf4KSectors + ExternalFlash::Config::NumberOf32KSectors + i - 1;
  }
  i = address >> ExternalFlash::Config::NumberOfAddressBitsIn32KbyteBlock;
  if (i >= 1) {
    assert(i >= 0 && i <= ExternalFlash::Config::NumberOf32KSectors);
    i = ExternalFlash::Config::NumberOf4KSectors + i - 1;
    return i;
  }
  i = address >> ExternalFlash::Config::NumberOfAddressBitsIn4KbyteBlock;
  assert(i <= ExternalFlash::Config::NumberOf4KSectors);
  return i;
}

int SectorAtAddress(uint32_t address) {
  if (InternalFlash::Config::StartAddress <= address && address < InternalFlash::Config::EndAddress) {
    return internalFlashSectorAtAddress(address);
  } else if (ExternalFlash::Config::StartAddress <= address && address < ExternalFlash::Config::EndAddress) {
    int i = externalFlashSectorAtAddress(address - ExternalFlash::Config::StartAddress);
    return i < 0 ? -1 : i + InternalFlash::Config::NumberOfSectors;
  } else {
    return -1;
  }
}

bool IncludesAddress(uint32_t address) {
  return (InternalFlash::Config::StartAddress <= address && address < InternalFlash::Config::EndAddress)
      || (ExternalFlash::Config::StartAddress <= address && address < ExternalFlash::Config::EndAddress);
}

}
}
}
