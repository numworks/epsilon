#include "flash_unprivileged.h"
#include "external_flash_unprivileged.h"
#include "internal_flash_unprivileged.h"
#include <drivers/config/external_flash.h>
#include <drivers/config/internal_flash.h>
#include <assert.h>
#include <stdint.h>

namespace Ion {
namespace Device {
namespace Flash {

bool AddressIsInInternalFlash(uint32_t address) {
  return address >= InternalFlash::Config::StartAddress
      && address <= InternalFlash::Config::EndAddress;
}

bool AddressIsInExternalFlash(uint32_t address) {
  return address >= ExternalFlash::Config::StartAddress
      && address <= ExternalFlash::Config::EndAddress;
}

int TotalNumberOfSectors() {
  return InternalFlash::Config::NumberOfSectors + ExternalFlash::Config::NumberOfSectors;
}

int SectorAtAddress(uint32_t address) {
  if (AddressIsInInternalFlash(address)) {
    return InternalFlash::SectorAtAddress(address);
  }
  if (AddressIsInExternalFlash(address)) {
    return InternalFlash::Config::NumberOfSectors + ExternalFlash::SectorAtAddress(address - ExternalFlash::Config::StartAddress);
  }
  return -1;
}

bool SectorIsInInternalFlash(int i) {
  return i >= 0 && i < InternalFlash::Config::NumberOfSectors;
}

bool SectorIsInExternalFlash(int i) {
  assert(i >= 0 && i < TotalNumberOfSectors());
  return i >= InternalFlash::Config::NumberOfSectors;
}

bool SectorIsWritableViaDFU(int i) {
  return SectorIsInExternalFlash(i) || (SectorIsInInternalFlash(i) && i >= InternalFlash::Config::NumberOfForbiddenFirstSectors);
}

}
}
}
