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

/* Once in jumping to pendsv_handler, the stack holds the exception context
 * frame but also miscellaneous values that were pushed in the setCheckpoint
 * method of the userland and kernel pendsv_handler_as/pendsv_handler.
 * The stack looks like this:
 *
 *        |     |
 *        |     |
 *       D+-----+D  <-- stack pointer stored by setjmp and restore by longjmp
 *        |     |
 *        |     |   <-- Miscellaneous values stored by pendsv_handler_as and
 *        |     |       pendsv_handler before executing setjmp
 *        |     |
 *       C+-----+C  <-- frame address
 *        |     |
 *        |Frame|   <-- Context frame stored on the stack to handle exception/
 *        |     |       thread mode switching
 *        |     |
 *       B+-----+B
 *        |     |   <-- Miscellaneous values stored by the userland
 *        |     |       Ion::CircuitBreaker API before the instruction SVC that
 *        |     |       will trigger the pendsv_handler
 *       A+-----+A
 *        |     |
 *        |     |
 *
 * We need to store/restore a stack snapshot from the point A to B to ensure
 * that right after the longjmp:
 * - the pendsv_handler ends correctly (C-D section)
 * - the pendsv_handler returns to the right method among the userland API
 *   Ion::CircuitBreaker (B-C section)
 * - the Ion::CircuitBreaker userland API returns correclty to the application
 *   code that used the Ion::CircuitBreaker API initially (A-B section)
 *
 * Empirically, the section A-B and C-D are:
 *                       +------------+--------------+------------+--------------+
 *                       |DEBUG=0/Home|DEBUG=0/Custom|DEBUG=1/Home|DEBUG=1/Custom|
 * +---------------------+------------+--------------+------------+--------------+
 * | Size in Bytes of CD |     20     |      20      |     36     |      36      |
 * | Size in bytes of AB |      0     |       8      |      0     |      24      |
 * +---------------------+------------+--------------+------------+--------------+
 *
 * We keep a storing space of 32 bytes (AB) + the frame size (BC) + 64 bytes (CD)
 * just to be sure.
 *
 * TODO: get the sp in Ion::CircuitBreaker::setCheckpoint to ensure the size of AB?
 *
 */

/* Context switching has to be done within a pendSV interrupt. Indeed, if the
 * interruption handling the context switching is nested within another
 * interruption, there is no easy way to inform the cortex of the interruption
 * nesting when loading a previous context (since the "active interruption"
 * registers of the cortex are read-only). We use pendSV to delay the context
 * switching until no other interruption remains to be executed. */

static constexpr size_t k_ABStackMaxSize = 32;
static constexpr size_t k_CDStackMaxSize = 64;
static constexpr size_t k_stackSnapshotMaxSize = k_ABStackMaxSize + k_extendedFrameSize + k_CDStackMaxSize;
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

static_assert(Ion::CircuitBreaker::k_checkpointBufferSize == sizeof(sCustomStackSnapshot) + sizeof(sCustomStackSnapshotAddress) + sizeof(sCustomStackSnapshotSize) + sizeof(sCustomRegisters) && Ion::CircuitBreaker::k_checkpointBufferSize == sizeof(sHomeStackSnapshot) + sizeof(sHomeStackSnapshotAddress) + sizeof(sHomeStackSnapshotSize) + sizeof(sHomeRegisters), "the CheckpointBuffer is either too small to hold the Checkpoint state or too big and is not optimal regarding RAM memory considerations");

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

bool busy() {
  return sPendingAction != PendingAction::None;
}

bool hasCheckpoint(Checkpoint checkpoint) {
  if (checkpoint == Checkpoint::Home) {
    return sHomeStackSnapshotAddress != nullptr;
  }
  assert(checkpoint == Checkpoint::Custom);
  return sCustomStackSnapshotAddress != nullptr;
}

void storeCustomCheckpoint(Ion::CircuitBreaker::CheckpointBuffer buffer) {
  assert(hasCheckpoint(Checkpoint::Custom));
  uint8_t * bufferPointer = static_cast<uint8_t *>(buffer);
  assert(buffer != nullptr);
  memcpy(bufferPointer, &sCustomRegisters, sizeof(jmp_buf));
  bufferPointer += sizeof(jmp_buf);
  memcpy(bufferPointer, &sCustomStackSnapshotSize, sizeof(size_t));
  bufferPointer += sizeof(size_t);
  memcpy(bufferPointer, &sCustomStackSnapshotAddress, sizeof(uint8_t *));
  bufferPointer += sizeof(uint8_t *);
  memcpy(bufferPointer, sCustomStackSnapshot, sCustomStackSnapshotSize);
}

void resetCustomCheckpoint(Ion::CircuitBreaker::CheckpointBuffer * buffer) {
  if (buffer == nullptr) {
    // Unset Custom checkpoint
    sCustomStackSnapshotAddress = nullptr;
  } else {
    uint8_t * bufferPointer = static_cast<uint8_t *>(*buffer);
    memcpy(&sCustomRegisters, bufferPointer, sizeof(jmp_buf));
    bufferPointer += sizeof(jmp_buf);
    memcpy(&sCustomStackSnapshotSize, bufferPointer, sizeof(size_t));
    bufferPointer += sizeof(size_t);
    memcpy(&sCustomStackSnapshotAddress, bufferPointer, sizeof(uint8_t *));
    bufferPointer += sizeof(uint8_t *);
    memcpy(sCustomStackSnapshot, bufferPointer, sCustomStackSnapshotSize);
  }
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

void setCheckpoint(Checkpoint checkpoint, bool overridePreviousCheckpoint) {
  if (!overridePreviousCheckpoint && hasCheckpoint(checkpoint)) {
    // Keep the oldest checkpoint
    return;
  }
  setPendingAction(checkpoint == Checkpoint::Custom ? PendingAction::SettingCustomCheckpoint : PendingAction::SettingHomeCheckpoint);
}

void loadCheckpoint(Checkpoint checkpoint) {
  assert(hasCheckpoint(checkpoint));
  setPendingAction(checkpoint == Checkpoint::Custom ? PendingAction::LoadingCustomCheckpoint : PendingAction::LoadingHomeCheckpoint);
}

}
}
}

using namespace Ion::Device::CircuitBreaker;

void pendsv_handler(uint8_t * frameAddress, uint32_t excReturn) {
  if (sPendingAction == PendingAction::LoadingCustomCheckpoint) {
      // Restore registers
      longjmp(sCustomRegisters, 1);
  } else if (sPendingAction == PendingAction::LoadingHomeCheckpoint) {
      // Restore registers
      longjmp(sHomeRegisters, 1);
  } else {
    // Extract current stack pointer
    uint8_t * stackPointerAddress = nullptr;
    asm volatile ("mov %[stackPointer], sp" : [stackPointer] "=r" (stackPointerAddress) :);

    // Store the stack frame
    if (sPendingAction == PendingAction::SettingCustomCheckpoint) {
      sCustomStackSnapshotAddress = stackPointerAddress;
      sCustomStackSnapshotSize = frameAddress + frameSize(excReturn) - stackPointerAddress + k_ABStackMaxSize;
      assert(sCustomStackSnapshotSize <= k_stackSnapshotMaxSize);
      memcpy(sCustomStackSnapshot, sCustomStackSnapshotAddress, sCustomStackSnapshotSize);
    } else {
      assert(sPendingAction == PendingAction::SettingHomeCheckpoint);
      sHomeStackSnapshotAddress = stackPointerAddress;
      sHomeStackSnapshotSize = frameAddress + frameSize(excReturn) - stackPointerAddress + k_ABStackMaxSize;
      assert(sHomeStackSnapshotSize <= k_stackSnapshotMaxSize);
      memcpy(sHomeStackSnapshot, sHomeStackSnapshotAddress, sHomeStackSnapshotSize);
    }

    // Store the interruption status cortex register
    jmp_buf * regsBufferAddress = sPendingAction == PendingAction::SettingCustomCheckpoint ? &sCustomRegisters : &sHomeRegisters;
    if (setjmp(*regsBufferAddress)) {
      /* WARNING: at this point the stack is corrupted. You can't use variable
       * stored on the stack. To prevent the compiler to do so, we ensure to
       * user global variables only which aren't stored on the stack. */
      // Restore stack frame
      if (sPendingAction == PendingAction::LoadingCustomCheckpoint) {
        memcpy(sCustomStackSnapshotAddress, sCustomStackSnapshot, sCustomStackSnapshotSize);
      } else {
        assert(sPendingAction == PendingAction::LoadingHomeCheckpoint);
        memcpy(sHomeStackSnapshotAddress, sHomeStackSnapshot, sHomeStackSnapshotSize);
      }
    }
  }

  // Update context switch state
  sLoading = sPendingAction == PendingAction::LoadingCustomCheckpoint || sPendingAction == PendingAction::LoadingHomeCheckpoint;
  sPendingAction = PendingAction::None;
}
