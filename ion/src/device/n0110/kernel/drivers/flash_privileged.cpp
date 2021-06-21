#include <shared/drivers/flash_privileged.h>
#include <kernel/drivers/authentication.h>
#include <kernel/drivers/board.h>
#include <shared/drivers/config/board.h>
#include <shared/drivers/config/external_flash.h>

namespace Ion {
namespace Device {
namespace Flash {

bool ForbiddenSector(int i) {
  int firstSectorSlotA = 0;
  int firstSectorSlotAAfterPersistingBytes = ExternalFlash::Config::NumberOf4KSectors + ExternalFlash::Config::NumberOf32KSectors;
  int firstSectorSlotB = SectorAtAddress(ExternalFlash::Config::StartAddress + ExternalFlash::Config::TotalSize/2);
  int firstSectorSlotBAfterPersistingBytes = firstSectorSlotB + 1;
  if (Authentication::trustedUserland()) {
    /* Authenticated userland can write:
     * - the persisting bytes of the current slot
     * - the external apps section of both slots
     * - the whole other slot except its persisting bytes. */
    if (Board::isRunningSlotA()) {
      return !((i >= firstSectorSlotA && i < firstSectorSlotAAfterPersistingBytes) ||
          (i >= firstSectorSlotB - Board::Config::ExternalAppsNumberOfSector && i < ExternalFlash::Config::NumberOfSectors));
    }
    return !(i >= firstSectorSlotBAfterPersistingBytes && i < ExternalFlash::Config::NumberOfSectors);
  }
  /* Non-authenticated userland can write:
   * - the external apps section of the running slot,
   * - the persisting bytes section of the running slot. */
  if (Board::isRunningSlotA()) {
    return !((i >= firstSectorSlotA && i < firstSectorSlotAAfterPersistingBytes) ||
        (i >= firstSectorSlotB - Board::Config::ExternalAppsNumberOfSector && i < firstSectorSlotB));
  }
  return !((i >= firstSectorSlotB && i < firstSectorSlotBAfterPersistingBytes) ||
    (i >= ExternalFlash::Config::NumberOfSectors - Board::Config::ExternalAppsNumberOfSector && i < ExternalFlash::Config::NumberOfSectors));
}


bool MassEraseEnable() {
  return false;
}

}
}
}
