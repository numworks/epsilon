#include <drivers/reset.h>
#include <ion/reset.h>
#include <shared/usb/calculator.h>
#include <userland/drivers/board.h>

namespace Ion {
namespace Device {
namespace USB {

void Calculator::leave(uint32_t leaveAddress) {
  // Reset the privileged mode
  //Board::switchExecutableSlot(leaveAddress);
  // Reinit the main stack pointer
  // Switch to privileged mode/main stack
  asm volatile ("mrs r0, control");
  /* The first bit defined the privileged mode, the second bit indicates to
   * use the Process Stack Pointer */
  asm volatile ("bic r0, #0x3");
  asm volatile ("msr control, r0");
  asm volatile ("isb 0xF");

  uint32_t * stackPointerAddress = reinterpret_cast<uint32_t *>(leaveAddress);
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
