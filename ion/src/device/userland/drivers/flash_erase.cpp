#include <drivers/svcall.h>
#include <shared/drivers/flash_erase.h>

namespace Ion {
namespace Device {
namespace Flash {

void SVC_ATTRIBUTES MassErase() {
  SVC_RETURNING_VOID(SVC_FLASH_MASS_ERASE)
}

bool SVC_ATTRIBUTES EraseSector(int i) {
  SVC_RETURNING_R0(SVC_FLASH_ERASE_SECTOR, bool)
}

}
}
}