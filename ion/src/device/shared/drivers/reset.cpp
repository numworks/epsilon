#include "reset.h"
#include <regs/regs.h>
#include <drivers/cache.h>
#include <drivers/board.h>
#include <drivers/external_flash.h>

extern void * _isr_start;

namespace Ion {
namespace Device {
namespace Reset {

using namespace Regs;

void core() {
  // Perform a full core reset
  CM4.AIRCR()->requestReset();
}

void jump() {
  // Disable cache before reset
  Ion::Device::Cache::disableDCache();
  Ion::Device::Cache::disableICache();

  /* Shutdown all clocks and periherals to mimic a hardware reset. Special
   * deinit for he external flash (it needs a reset command). */
  ExternalFlash::deinit();
  Board::shutdown();

  /* Jump to the reset service routine after having reset the stack pointer.
   * Both addresses are fetched from the base of the Flash memory, just like a
   * real reset would. These operations should be made at once, otherwise the C
   * compiler might emit some instructions that modify the stack inbetween. */

  uint32_t * stackPointerAddress = reinterpret_cast<uint32_t *>(&_isr_start);
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
