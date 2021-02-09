#include <drivers/circuit_breaker.h>
#include <drivers/cache.h>
#include <kernel/boot/isr.h>
#include <regs/regs.h>
#include <assert.h>
#include <setjmp.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>


namespace Ion {
namespace Device {
namespace CircuitBreaker {

/* Basic frame description:
 * | r0 | r1 | r2 | r3 | r12 | lr | return address | xPSR | */
static constexpr size_t k_basicFrameSize = 0x20;

/* Extended frame description:
 * | r0 | r1 | r2 | r3 | r12 | lr | return address | xPSR | s0 | s1 | ... | s 15 | FPSCR | */

static constexpr size_t k_extendedFrameSize = 0x68;

/* Once in jumping to setCheckpoint, the stack holds the exception context
 * frame but also miscellaneous values that were pushed in the svc_call_handler.
 * The stack looks like this:
 *
 *        |     |
 *        |     |
 *        +-----+   <-- stack pointer
 *        |     |
 *        |     |
 *        |     |
 *        +-----+
 *        |     |
 *        |Frame|
 *        |     |
 *        +-----+   <-- frame origin
 *        |     |
 *        |     |
 *        +-----+
 *
 * We need to store a stack snapshot from the frame origin to the current stack
 * pointer. Empirically, the additional values pushed on the stack on the top
 * of the frame need up to 144 bytes when we compile with optimizations. We
 * keep a storing space of 256 bytes + the frame size just to be sure.  */

static constexpr size_t k_exceptionStackMaxSize = 256;
uint8_t sExceptionStack[k_extendedFrameSize + k_exceptionStackMaxSize];
uint8_t * sExceptionStackAddress = nullptr;
size_t sExceptionStackSize;
jmp_buf sRegisters;

size_t frameSize(uint32_t excReturn) {
  /* The exception return value indicating the return context:
   * - the stack frame used (FPU extended?),
   * - the stack pointer used (MSP/PSP)
   * - the mode used (thread/handler)
   *  We extract the type
   */
  bool useBasicFrame = excReturn & 0x10;
  return useBasicFrame ? k_basicFrameSize : k_extendedFrameSize;
}

bool hasCheckpoint() {
  return sExceptionStackAddress != nullptr;
}

void setCheckpoint(uint8_t * frameAddress, uint32_t excReturn) {
  if (hasCheckpoint()) {
    // Keep the oldest checkpoint
    return;
  }
  // TODO EMILIE: Disable other exceptions or change priority!

  // Extract current stack pointer
  uint8_t * stackPointerAddress = nullptr;
  asm volatile ("mov %[stackPointer], sp" : [stackPointer] "=r" (stackPointerAddress) :);

  // Store the stack frame
  sExceptionStackAddress = stackPointerAddress;
  sExceptionStackSize = frameAddress + frameSize(excReturn) - stackPointerAddress;
  assert(sExceptionStackSize <= k_exceptionStackMaxSize);
  memcpy(sExceptionStack, sExceptionStackAddress, sExceptionStackSize);

  if (setjmp(sRegisters)) {
    // Restore stack frame
    memcpy(sExceptionStackAddress, sExceptionStack, sExceptionStackSize);
    // Reset checkpoint
    unsetCheckpoint();
  }
}

void unsetCheckpoint() {
  sExceptionStackAddress = nullptr;
}

void loadCheckpoint(uint8_t * frameAddress) {
  // TODO EMILIE: Disable other exceptions or change priority!
  assert(hasCheckpoint());

  // Restore registers
  longjmp(sRegisters, 1);
}

}
}
}
