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

int SectorAtAddress(uint32_t address) {
  if (address >= InternalFlash::Config::StartAddress
      && address <= InternalFlash::Config::EndAddress)
  {
    return InternalFlash::SectorAtAddress(address);
  }
  if (address >= ExternalFlash::Config::StartAddress
      && address <= ExternalFlash::Config::EndAddress)
  {
    return InternalFlash::Config::NumberOfSectors + ExternalFlash::SectorAtAddress(address - ExternalFlash::Config::StartAddress);
  }
  return -1;
}

void MassErase() {
  InternalFlash::MassErase();
  ExternalFlash::MassErase();
}

void EraseSector(int i) {
  assert(i >= 0 && i < TotalNumberOfSectors());
  if (i < InternalFlash::Config::NumberOfSectors) {
    InternalFlash::EraseSector(i);
  } else {
    ExternalFlash::EraseSector(i - InternalFlash::Config::NumberOfSectors);
  }
}

void WriteMemory(uint8_t * destination, uint8_t * source, size_t length) {
  assert(SectorAtAddress((uint32_t)destination) >= 0);
  if (SectorAtAddress((uint32_t)destination) < InternalFlash::Config::NumberOfSectors) {
    InternalFlash::WriteMemory(destination, source, length);
  } else {
    ExternalFlash::WriteMemory(destination, source, length);
  }
}

void DisableInternalProtection() {
  InternalFlash::DisableProtection();
}

void EnableInternalProtection() {
  InternalFlash::EnableProtection();
}

void SetInternalSectorProtection(int i, bool protect) {
  InternalFlash::SetSectorProtection(i, protect);
}

void LockInternalFlashForSession() {
  InternalFlash::EnableSessionLock();
}

void EnableInternalFlashInterrupt() {
  InternalFlash::EnableFlashInterrupt();
}

void ClearInternalFlashErrors() {
  InternalFlash::ClearErrors();
}

void LockSlotA() {
  ExternalFlash::LockSlotA();
}

void LockSlotB() {
  ExternalFlash::LockSlotB();
}

}
}
}
