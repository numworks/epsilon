#include <shared/drivers/external_flash_write.h>
#include <shared/drivers/trampoline.h>
#include <assert.h>

namespace Ion {
namespace Device {
namespace ExternalFlash {

void MassErase() {
  // Mass erase is not enabled on kernel
  assert(false);
}

TRAMPOLINE_INTERFACE(TRAMPOLINE_EXTERNAL_FLASH_ERASE_SECTOR, EraseSector, (i), void, int i)
TRAMPOLINE_INTERFACE(TRAMPOLINE_EXTERNAL_FLASH_WRITE_MEMORY, WriteMemory, (destination, source, length), void, uint8_t * destination, const uint8_t * source, size_t length)

}
}
}
