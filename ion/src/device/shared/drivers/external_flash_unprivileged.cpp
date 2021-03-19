#include "external_flash_unprivileged.h"
#include <drivers/config/external_flash.h>
#include <assert.h>

namespace Ion {
namespace Device {
namespace ExternalFlash {

int SectorAtAddress(uint32_t address) {
  /* WARNING: this code assumes that the flash sectors are of increasing size:
   * first all 4K sectors, then all 32K sectors, and finally all 64K sectors. */
  int i = address >> NumberOfAddressBitsIn64KbyteBlock;
  if (i > Config::NumberOf64KSectors) {
    return -1;
  }
  if (i >= 1) {
    return Config::NumberOf4KSectors + Config::NumberOf32KSectors + i - 1;
  }
  i = address >> NumberOfAddressBitsIn32KbyteBlock;
  if (i >= 1) {
    assert(i >= 0 && i <= Config::NumberOf32KSectors);
    i = Config::NumberOf4KSectors + i - 1;
    return i;
  }
  i = address >> NumberOfAddressBitsIn4KbyteBlock;
  assert(i <= Config::NumberOf4KSectors);
  return i;
}

}
}
}
