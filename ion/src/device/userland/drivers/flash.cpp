#include <shared/drivers/flash_privileged.h>
#include <userland/drivers/svcall.h>

namespace Ion {
namespace Device {
namespace Flash {

void SVC_ATTRIBUTES MassErase() {
  SVC(SVC_FLASH_MASS_ERASE);
}

bool SVC_ATTRIBUTES EraseSector(int i) {
  SVC(SVC_FLASH_ERASE_SECTOR);
}

bool SVC_ATTRIBUTES WriteMemory(uint8_t * destination, const uint8_t * source, size_t length) {
  SVC(SVC_FLASH_WRITE_MEMORY);
}

}
}
}
