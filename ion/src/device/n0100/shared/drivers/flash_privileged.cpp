#include <shared/drivers/flash_privileged.h>
#include <shared/drivers/internal_flash.h>

namespace Ion {
namespace Device {
namespace Flash {

using namespace Regs;

bool ForbiddenSector(int i) {
  return i < NumberOfForbiddenFirstSectors; // TODO: how big is the bootloader?
}

bool MassEraseEnable() {
  return false;
}

void EraseSector(int i) {
  return InternalFlash::EraseSector(i);
}

void WriteMemory(uint8_t * destination, uint8_t * source, size_t length) {
  return InternalFlash::WriteMemory(destination, source, length);
}

}
}
}
