#include <drivers/board.h>
#include <drivers/reset.h>
#include <ion/reset.h>
#include <shared/usb/calculator.h>

namespace Ion {
namespace Device {
namespace USB {

void Calculator::leave(uint32_t leaveAddress) {
  // Switch back to the main stack
  asm volatile ("mrs r0, control");
  asm volatile ("bic r0, #0x2");
  asm volatile ("msr control, r0");
  asm volatile ("isb 0xF");

  // Jump
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
