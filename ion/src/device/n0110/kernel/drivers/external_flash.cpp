#include <shared/drivers/external_flash_write.h>
#include <shared/drivers/trampoline.h>
#include <assert.h>

namespace Ion {
namespace Device {
namespace ExternalFlash {

TRAMPOLINE_INTERFACE(TRAMPOLINE_EXTERNAL_FLASH_ERASE_SECTOR, PrivateEraseSector, (i), void, int i)
TRAMPOLINE_INTERFACE(TRAMPOLINE_EXTERNAL_FLASH_WRITE_MEMORY, PrivateWriteMemory, (destination, source, length), void, uint8_t * destination, const uint8_t * source, size_t length)

using namespace Regs;

void MassErase() {
  // Mass erase is not enabled on kernel
  assert(false);
}

void WriteMemory(uint8_t * destination, const uint8_t * source, size_t length) {
  /* Ideally, we should assert that all interruptions are disabled since their
   * handler code might be located in the external flash. Writing/erasing the
   * external flash requires to priorly stop the memory-mapped mode preventing
   * executing any code living there. In practice, we only assert the the
   * systick is off because all interruptions are switched on and off at the
   * same time. */
  assert(!CORTEX.SYST_CSR()->getTICKINT());
  PrivateWriteMemory(destination, source, length);
}

void EraseSector(int i) {
  // See comment in WriteMemory
  assert(!CORTEX.SYST_CSR()->getTICKINT());
  PrivateEraseSector(i);
}

}
}
}
