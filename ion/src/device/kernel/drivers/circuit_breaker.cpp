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

using namespace Regs;

/* Basic frame description:
 * | r0 | r1 | r2 | r3 | r12 | lr | return address | xPSR | Reserved | Reserved | */
static constexpr size_t k_basicFrameSize = 0x28;

/* Extended frame description:
 * | r0 | r1 | r2 | r3 | r12 | lr | return address | xPSR | s0 | s1 | ... | s 15 | FPSCR | Reserved | Reserved | Reserved | */
static constexpr size_t k_extendedFrameSize = 0x70;

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


/* Context switching has to be done within a pendSV interrupt. Indeed, if the
 * interruption handling the context switching is nested within another
 * interruptions, there is no easy way to inform the cortex of the interruption
 * nesting when loading a previous context (since the "active interruption"
 * registers of the cortex are read-only). We use pendSV to delay the context
 * switching until no other interruption remains to be executed. */

static constexpr size_t k_additionalStackSnapshotMaxSize = 256;
static constexpr size_t k_stackSnapshotMaxSize = k_additionalStackSnapshotMaxSize + k_extendedFrameSize;
static_assert(k_basicFrameSize < k_extendedFrameSize, "The basic exception frame size is smaller than the extended exception frame.");

enum class PendingAction {
  None,
  SettingCustomCheckpoint,
  LoadingCustomCheckpoint,
  SettingHomeCheckpoint,
  LoadingHomeCheckpoint
};

PendingAction sPendingAction = PendingAction::None;
bool sLoading = false;

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

inline size_t frameSize(uint32_t excReturn) {
  /* The exception return value indicating the return context:
   * - the stack frame used (FPU extended?),
   * - the stack pointer used (MSP/PSP)
   * - the mode used (thread/handler)
   *  We extract the type
   */
  bool useBasicFrame = excReturn & 0x10;
  return useBasicFrame ? k_basicFrameSize : k_extendedFrameSize;
}

inline uint8_t * stackSnapshot(Checkpoint checkpoint) {
  return checkpoint == Checkpoint::Custom ? sCustomStackSnapshot : sHomeStackSnapshot;
}

inline uint8_t ** stackSnapshotAddress(Checkpoint checkpoint) {
  return checkpoint == Checkpoint::Custom ? &sCustomStackSnapshotAddress : &sHomeStackSnapshotAddress;
}

inline size_t * stackSnapshotSize(Checkpoint checkpoint) {
  return checkpoint == Checkpoint::Custom ? &sCustomStackSnapshotSize : &sHomeStackSnapshotSize;
}

inline jmp_buf * registers(Checkpoint checkpoint) {
  return checkpoint == Checkpoint::Custom ? &sCustomRegisters : &sHomeRegisters;
}

bool hasCheckpoint(Checkpoint checkpoint) {
  return *stackSnapshotAddress(checkpoint) != nullptr;
}

void unsetCustomCheckpoint() {
  *stackSnapshotAddress(Checkpoint::Custom) = nullptr;
}

bool clearCheckpointFlag(Checkpoint checkpoint) {
  bool currentLoadingStatus = sLoading;
  sLoading = false;
  return currentLoadingStatus;
}

static inline void setPendingAction(PendingAction action) {
  assert(sPendingAction == PendingAction::None || sPendingAction == action);
  sPendingAction = action;
  Ion::Device::Regs::CORTEX.ICSR()->setPENDSVSET(true);
}

void setCheckpoint(Checkpoint checkpoint) {
  setPendingAction(checkpoint == Checkpoint::Custom ? PendingAction::SettingCustomCheckpoint : PendingAction::SettingHomeCheckpoint);
}

void loadCheckpoint(Checkpoint checkpoint) {
  assert(hasCheckpoint(checkpoint));
  setPendingAction(checkpoint == Checkpoint::Custom ? PendingAction::LoadingCustomCheckpoint : PendingAction::LoadingHomeCheckpoint);
}

void privateSetCheckpoint(Checkpoint checkpoint, uint8_t * frameAddress, uint32_t excReturn) {
  if (hasCheckpoint(checkpoint)) {
    // Keep the oldest checkpoint
    return;
  }
  // Extract current stack pointer
  uint8_t * stackPointerAddress = nullptr;
  asm volatile ("mov %[stackPointer], sp" : [stackPointer] "=r" (stackPointerAddress) :);

  // Store the stack frame
  *stackSnapshotAddress(checkpoint) = stackPointerAddress;
  *stackSnapshotSize(checkpoint) = frameAddress + frameSize(excReturn) - stackPointerAddress;
  assert(*stackSnapshotSize(checkpoint) <= k_stackSnapshotMaxSize);
  memcpy(stackSnapshot(checkpoint), *stackSnapshotAddress(checkpoint), *stackSnapshotSize(checkpoint));

  // Store the interruption status cortex register
  if (setjmp(*registers(checkpoint))) {
    /* WARNING: you can't use previous local variables: they might be stored on the stack which hasn't been restored yet! */
    // Restore stack frame
    memcpy(*stackSnapshotAddress(checkpoint), stackSnapshot(checkpoint), *stackSnapshotSize(checkpoint));
  }
}

void privateLoadCheckpoint(Checkpoint checkpoint) {
  // Restore registers
  longjmp(*registers(checkpoint), 1);
}

}
}
}

using namespace Ion::Device::CircuitBreaker;

void pendsv_handler(uint8_t * frameAddress, uint32_t excReturn) {
  switch (sPendingAction) {
    case PendingAction::SettingCustomCheckpoint:
      privateSetCheckpoint(Checkpoint::Custom, frameAddress, excReturn);
      break;
    case PendingAction::SettingHomeCheckpoint:
      privateSetCheckpoint(Checkpoint::Home, frameAddress, excReturn);
      break;
    case PendingAction::LoadingCustomCheckpoint:
      privateLoadCheckpoint(Checkpoint::Custom);
      break;
    case PendingAction::LoadingHomeCheckpoint:
      privateLoadCheckpoint(Checkpoint::Home);
      break;
    default:
      assert(false);
  }
  sLoading = sPendingAction == PendingAction::LoadingCustomCheckpoint || sPendingAction == PendingAction::LoadingHomeCheckpoint;
  sPendingAction = PendingAction::None;
}
