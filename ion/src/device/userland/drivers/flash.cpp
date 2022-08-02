#include <drivers/svcall.h>
#include <shared/drivers/flash.h>

namespace Ion {
namespace Device {
namespace Flash {

void SVC_ATTRIBUTES MassErase() {
  SVC_RETURNING_VOID(SVC_FLASH_MASS_ERASE)
}

bool SVC_ATTRIBUTES EraseSector(int i) {
  SVC_RETURNING_R0(SVC_FLASH_ERASE_SECTOR, bool)
}

bool SVC_ATTRIBUTES WriteMemory(uint8_t * destination, const uint8_t * source, size_t length) {
  SVC_RETURNING_R0(SVC_FLASH_WRITE_MEMORY, bool)
}

}
}
}
