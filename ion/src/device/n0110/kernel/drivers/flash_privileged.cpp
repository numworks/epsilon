#include <shared/drivers/flash_privileged.h>
#include <kernel/drivers/board.h>
#include <shared/drivers/config/external_flash.h>

namespace Ion {
namespace Device {
namespace Flash {

bool ForbiddenSector(int i) {
  bool firstSlotUsed = Board::userlandStart() < ExternalFlash::Config::StartAddress + ExternalFlash::Config::TotalSize/2;
  if (firstSlotUsed) {
    return i < ExternalFlash::Config::NumberOfSectors/2 || i >= ExternalFlash::Config::NumberOfSectors;
  }
  return i < 0 || i >= ExternalFlash::Config::NumberOfSectors/2;
}

bool MassEraseEnable() {
  return false;
}

}
}
}
