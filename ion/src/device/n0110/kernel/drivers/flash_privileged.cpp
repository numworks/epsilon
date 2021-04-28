#include <shared/drivers/flash_privileged.h>
#include <kernel/drivers/authentication.h>
#include <kernel/drivers/board.h>
#include <shared/drivers/config/board.h>
#include <shared/drivers/config/external_flash.h>

namespace Ion {
namespace Device {
namespace Flash {

bool ForbiddenSector(int i) {
  if (Authentication::trustedUserland()) {
    /* Authenticated userland can write the external apps section of both slots
     * and the whole other slot. */
    if (Board::isRunningSlotA()) {
      return i < ExternalFlash::Config::NumberOfSectors/2 - Board::Config::ExternalAppsNumberOfSector || i >= ExternalFlash::Config::NumberOfSectors;
    }
    return i < 0 || (i >= ExternalFlash::Config::NumberOfSectors/2 && i < ExternalFlash::Config::NumberOfSectors - Board::Config::ExternalAppsNumberOfSector) || i >= ExternalFlash::Config::NumberOfSectors;
  }
  /* Non-authenticated userland can only write the external apps section of the
   * running slot. */
  if (Board::isRunningSlotA()) {
    return i < ExternalFlash::Config::NumberOfSectors/2 - Board::Config::ExternalAppsNumberOfSector || i >= ExternalFlash::Config::NumberOfSectors/2;
  }
  return i < ExternalFlash::Config::NumberOfSectors - Board::Config::ExternalAppsNumberOfSector || i >= ExternalFlash::Config::NumberOfSectors;
}


bool MassEraseEnable() {
  return false;
}

}
}
}
