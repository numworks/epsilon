#include "flash.h"
#include "external_flash.h"
#include "internal_flash.h"
#include <drivers/config/internal_flash.h>
#include <drivers/config/external_flash.h>
#include <assert.h>

namespace Ion {
namespace Device {
namespace Flash {

int TotalNumberOfSectors() {
  return InternalFlash::Config::NumberOfSectors + ExternalFlash::Config::NumberOfSectors;
}

bool AddressIsInInternalFlash(uint32_t address) {
  return address >= InternalFlash::Config::StartAddress
      && address <= InternalFlash::Config::EndAddress;
}

bool AddressIsInExternalFlash(uint32_t address) {
  return address >= ExternalFlash::Config::StartAddress
      && address <= ExternalFlash::Config::EndAddress;
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

