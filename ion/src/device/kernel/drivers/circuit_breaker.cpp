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

static constexpr size_t k_additionalStackSnapshotMaxSize = 256;
static constexpr size_t k_stackSnapshotMaxSize = k_additionalStackSnapshotMaxSize + k_extendedFrameSize;
static_assert(k_basicFrameSize < k_extendedFrameSize, "The basic exception frame size is smaller than the extended exception frame.");

// Home checkpoint snapshot
uint8_t sHomeStackSnapshot[k_stackSnapshotMaxSize];
uint8_t * sHomeStackSnapshotAddress = nullptr;
size_t sHomeStackSnapshotSize;
jmp_buf sHomeRegisters;

// Custom checkpoint snapshot
uint8_t sCustomStackSnapshot[k_stackSnapshotMaxSize];
uint8_t * sCustomStackSnapshotAddress = nullptr;
size_t sCustomStackSnapshotSize;
jmp_buf sCustomRegisters;

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

bool hasCheckpoint(Ion::CircuitBreaker::Checkpoint checkpoint) {
  uint8_t * stackSnapshotAddress = checkpoint == Ion::CircuitBreaker::Checkpoint::Home ? sHomeStackSnapshotAddress : sCustomStackSnapshotAddress;
  return stackSnapshotAddress != nullptr;
}

void setCheckpoint(Ion::CircuitBreaker::Checkpoint checkpoint, uint8_t * frameAddress, uint32_t excReturn) {
  if (Ion::Device::CircuitBreaker::hasCheckpoint(checkpoint)) {
    // Keep the oldest checkpoint
    return;
  }

  // Extract current stack pointer
  uint8_t * stackPointerAddress = nullptr;
  asm volatile ("mov %[stackPointer], sp" : [stackPointer] "=r" (stackPointerAddress) :);

  // Choose the right checkpoint snapshot
  uint8_t * stackSnapshot = sHomeStackSnapshot;
  uint8_t ** stackSnapshotAddress = &sHomeStackSnapshotAddress;
  size_t * stackSnapshotSize = &sHomeStackSnapshotSize;
  jmp_buf * registers = &sHomeRegisters;
  if (checkpoint == Ion::CircuitBreaker::Checkpoint::Custom) {
    stackSnapshot = sCustomStackSnapshot;
    stackSnapshotAddress = &sCustomStackSnapshotAddress;
    stackSnapshotSize = &sCustomStackSnapshotSize;
    registers = &sCustomRegisters;
  }

  // Store the stack frame
  *stackSnapshotAddress = stackPointerAddress;
  *stackSnapshotSize = frameAddress + frameSize(excReturn) - stackPointerAddress;
  assert(*stackSnapshotSize <= k_stackSnapshotMaxSize);
  memcpy(stackSnapshot, *stackSnapshotAddress, *stackSnapshotSize);

  if (setjmp(*registers)) {
    // Restore stack frame
    memcpy(*stackSnapshotAddress, stackSnapshot, *stackSnapshotSize);
    // Reset checkpoint
    unsetCheckpoint(checkpoint);
  }
}

void unsetCheckpoint(Ion::CircuitBreaker::Checkpoint checkpoint) {
  uint8_t ** stackSnapshotAddress = checkpoint == Ion::CircuitBreaker::Checkpoint::Home ? &sHomeStackSnapshotAddress : &sCustomStackSnapshotAddress;
  *stackSnapshotAddress = nullptr;
}

void loadCheckpoint(Ion::CircuitBreaker::Checkpoint checkpoint, uint8_t * frameAddress) {
  assert(Ion::Device::CircuitBreaker::hasCheckpoint(checkpoint));

  // Restore registers
  longjmp(checkpoint == Ion::CircuitBreaker::Checkpoint::Home ? sHomeRegisters : sCustomRegisters, 1);
}

}
}
}
