#include <shared/drivers/external_flash_write.h>
#include <shared/drivers/trampoline.h>
#include <assert.h>

namespace Ion {
namespace Device {
namespace ExternalFlash {

TRAMPOLINE_INTERFACE(TRAMPOLINE_EXTERNAL_FLASH_ERASE_SECTOR, PrivateEraseSector, (i), void, int i)
TRAMPOLINE_INTERFACE(TRAMPOLINE_EXTERNAL_FLASH_WRITE_MEMORY, PrivateWriteMemory, (destination, source, length), void, uint8_t * destination, const uint8_t * source, size_t length)

void MassErase() {
  // Mass erase is not enabled on kernel
  assert(false);
}

void WriteMemory(uint8_t * destination, const uint8_t * source, size_t length) {
  //Board::shutdownInterruptions();
  PrivateWriteMemory(destination, source, length);
  //Board::initInterruptions();
}

void EraseSector(int i) {
  //Board::shutdownInterruptions();
  PrivateEraseSector(i);
  //Board::initInterruptions();
}

}
}
}
