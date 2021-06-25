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
    /* Authenticated userland can write:
     * - the persisting bytes of both slots
     * - the external apps section of both slots
     * - the whole other slot. */
    int firstForbiddenSector = SectorAtAddress(reinterpret_cast<uint32_t>(Board::kernelHeader()));
    int lastForbiddenSector = SectorAtAddress(reinterpret_cast<uint32_t>(Board::userlandHeader()->storageAddressFlash()));
    return i >= firstForbiddenSector && i < lastForbiddenSector;
  }
  /* Non-authenticated userland can write:
   * - the external apps section of the running slot,
   * - the persisting bytes section of the running slot. */
  uint32_t storageAddress = reinterpret_cast<uint32_t>(Board::userlandHeader()->storageAddressFlash());
  int firstSectorExternalApps = SectorAtAddress(storageAddress);
  int lastSectorExternalApps = SectorAtAddress(storageAddress + Board::userlandHeader()->storageSizeFlash());
  // We add the persisting bytes sector
  int lastAuthorizedSector = lastSectorExternalApps + 1;
  return !(i >= firstSectorExternalApps && i <= lastAuthorizedSector);
}

bool MassEraseEnable() {
  return false;
}

}
}
}
