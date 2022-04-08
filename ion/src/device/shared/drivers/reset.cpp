#include "reset.h"
#include <regs/regs.h>
#include <drivers/cache.h>
#include <drivers/board.h>
#include <drivers/external_flash.h>

namespace Ion {
namespace Device {
namespace Reset {

using namespace Regs;

void core() {
  // Perform a full core reset
  Ion::Device::Cache::dsb(); // Complete all memory accesses
  CORTEX.AIRCR()->requestReset();
  Ion::Device::Cache::dsb();
  // Wait until reset
  while (true) {
    asm("nop");
  }
}

/* We isolate the jump code that needs to be executed from the internal
 * flash (because the external flash is then shut down). We forbid
 * inlining these instructions in the external flash. */

void __attribute__((noinline)) internalFlashJump(uint32_t jumpIsrVectorAddress) {
  ExternalFlash::shutdown();
  Board::shutdownClocks();

  /* Jump to the reset service routine after having reset the stack pointer.
   * Both addresses are fetched from the base of the Flash memory, just like a
   * real reset would. These operations should be made at once, otherwise the C
   * compiler might emit some instructions that modify the stack inbetween. */

  uint32_t * stackPointerAddress = reinterpret_cast<uint32_t *>(jumpIsrVectorAddress);
  uint32_t * resetHandlerAddress = stackPointerAddress + 1;

  asm volatile (
      "msr MSP, %[stackPointer] ; bx %[resetHandler]"
      : :
      [stackPointer] "r" (*stackPointerAddress),
      [resetHandler] "r" (*resetHandlerAddress)
  );
}

void jump(uint32_t jumpIsrVectorAddress) {
  // Disable cache before reset
  Ion::Device::Cache::disable();

  /* Shutdown all clocks and peripherals to mimic a hardware reset. */
  Board::shutdownPeripherals();
  internalFlashJump(jumpIsrVectorAddress);

}

}
}
}
