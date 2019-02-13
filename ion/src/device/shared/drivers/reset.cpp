#include "reset.h"
#include <regs/regs.h>

namespace Ion {
namespace Device {
namespace Reset {

using namespace Regs;

void core() {
  // Perform a full core reset
  CM4.AIRCR()->requestReset();
}

void jump() {
  uint32_t * stackPointerAddress = reinterpret_cast<uint32_t *>(0x08000000);
  uint32_t * resetHandlerAddress = reinterpret_cast<uint32_t *>(0x08000004);

  /* Jump to the reset service routine after having reset the stack pointer.
   * Both addresses are fetched from the base of the Flash memory, just like a
   * real reset would. These operations should be made at once, otherwise the C
   * compiler might emit some instructions that modify the stack inbetween. */

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
