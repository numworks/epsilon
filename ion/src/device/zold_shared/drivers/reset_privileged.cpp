#include "reset_privileged.h"
#include <regs/regs.h>
#include <drivers/cache.h>
#include <drivers/board_privileged.h>
#include <drivers/external_flash.h>

namespace Ion {
namespace Device {
namespace Reset {

using namespace Regs;

void core() {
  coreHandler();
}

void coreHandler() {
  // Perform a full core reset
  Ion::Device::Cache::dsb(); // Complete all memory accesses
  CORTEX.AIRCR()->requestReset();
  Ion::Device::Cache::dsb();
  // Wait until reset
  while (true) {
    asm("nop");
  }
}


/* jump is executed from the internal flash only as it shutdowns the external
 * flash. */

void jump(uint32_t jumpIsrVectorAddress) {
  // Disable cache before reset
  Ion::Device::Cache::disable();

  /* Shutdown all clocks and periherals to mimic a hardware reset. */
  Board::shutdownPeripherals();
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

}
}
}
