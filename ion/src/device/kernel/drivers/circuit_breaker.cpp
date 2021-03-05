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
using namespace Ion::CircuitBreaker;

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

enum class InternalStatus {
  Set,
  Loaded,
  PendingSetCheckpoint,
  PendingLoadCheckpoint,
};

InternalStatus sInternalStatus = InternalStatus::Loaded;
CheckpointType sCheckpointType = CheckpointType::Home;

// Home checkpoint snapshot
uint8_t sHomeStackSnapshot[k_stackSnapshotMaxSize];
uint8_t * sHomeStackSnapshotAddress = nullptr;
size_t sHomeStackSnapshotSize;
jmp_buf sHomeRegisters;

// User checkpoint snapshot
uint8_t sUserStackSnapshot[k_stackSnapshotMaxSize];
uint8_t * sUserStackSnapshotAddress = nullptr;
size_t sUserStackSnapshotSize;
jmp_buf sUserRegisters;

// System checkpoint snapshot
uint8_t sSystemStackSnapshot[k_stackSnapshotMaxSize];
uint8_t * sSystemStackSnapshotAddress = nullptr;
size_t sSystemStackSnapshotSize;
jmp_buf sSystemRegisters;

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

Ion::CircuitBreaker::Status status() {
  if (sInternalStatus == InternalStatus::Set) {
   return Status::Set;
  } else if (sInternalStatus == InternalStatus::Loaded) {
    return Status::Interrupted;
  } else {
    return Status::Busy;
  }
}

bool hasCheckpoint(CheckpointType type) {
  switch (type) {
    case CheckpointType::Home:
      return sHomeStackSnapshotAddress != nullptr;
    case CheckpointType::User:
      return sUserStackSnapshotAddress != nullptr;
    default:
      assert(type == CheckpointType::System);
      return sSystemStackSnapshotAddress != nullptr;
  }
}

void unsetCheckpoint(CheckpointType type) {
  switch (type) {
    case CheckpointType::Home:
      sHomeStackSnapshotAddress = nullptr;
      return;
    case CheckpointType::User:
      sUserStackSnapshotAddress = nullptr;
      return;
    default:
      assert(type == CheckpointType::System);
      sSystemStackSnapshotAddress = nullptr;
      return;
  }
}

static inline void setPendingAction(CheckpointType type, InternalStatus action) {
  assert(sInternalStatus == InternalStatus::Loaded || sInternalStatus == InternalStatus::Set);
  sCheckpointType = type;
  sInternalStatus = action;
  Ion::Device::Regs::CORTEX.ICSR()->setPENDSVSET(true);
}

bool setCheckpoint(CheckpointType type) {
  if (Device::CircuitBreaker::hasCheckpoint(type)) {
    // Keep the oldest checkpoint
    return false;
  }
  setPendingAction(type, InternalStatus::PendingSetCheckpoint);
  return true;
}

void loadCheckpoint(CheckpointType type) {
  assert(Device::CircuitBreaker::hasCheckpoint(type));
  setPendingAction(type, InternalStatus::PendingLoadCheckpoint);
}

}
}
}

using namespace Ion::Device::CircuitBreaker;

void pendsv_handler(uint8_t * frameAddress, uint32_t excReturn) {
  // status is supposed to be one of the pending status
  assert(sInternalStatus == InternalStatus::PendingSetCheckpoint || sInternalStatus == InternalStatus::PendingLoadCheckpoint);

  if (sInternalStatus == InternalStatus::PendingLoadCheckpoint) {
    switch (sCheckpointType) {
      case CheckpointType::Home:
        longjmp(sHomeRegisters, 1);
      case CheckpointType::User:
        longjmp(sUserRegisters, 1);
      default:
        assert(sCheckpointType == CheckpointType::System);
        longjmp(sSystemRegisters, 1);
    }
  } else {
    assert(sInternalStatus == InternalStatus::PendingSetCheckpoint);
    // Extract current stack pointer
    uint8_t * stackPointerAddress = nullptr;
    asm volatile ("mov %[stackPointer], sp" : [stackPointer] "=r" (stackPointerAddress) :);

    // Store the stack frame
    jmp_buf * regsBufferAddress;
    if (sCheckpointType == CheckpointType::Home) {
      regsBufferAddress = &sHomeRegisters;
      sHomeStackSnapshotAddress = stackPointerAddress;
      sHomeStackSnapshotSize = frameAddress + frameSize(excReturn) - stackPointerAddress + k_ABStackMaxSize;
      assert(sHomeStackSnapshotSize <= k_stackSnapshotMaxSize);
      memcpy(sHomeStackSnapshot, sHomeStackSnapshotAddress, sHomeStackSnapshotSize);
    } else if (sCheckpointType == CheckpointType::User) {
      regsBufferAddress = &sUserRegisters;
      sUserStackSnapshotAddress = stackPointerAddress;
      sUserStackSnapshotSize = frameAddress + frameSize(excReturn) - stackPointerAddress + k_ABStackMaxSize;
      assert(sUserStackSnapshotSize <= k_stackSnapshotMaxSize);
      memcpy(sUserStackSnapshot, sUserStackSnapshotAddress, sUserStackSnapshotSize);
    } else {
      assert(sCheckpointType == CheckpointType::System);
      regsBufferAddress = &sSystemRegisters;
      sSystemStackSnapshotAddress = stackPointerAddress;
      sSystemStackSnapshotSize = frameAddress + frameSize(excReturn) - stackPointerAddress + k_ABStackMaxSize;
      assert(sSystemStackSnapshotSize <= k_stackSnapshotMaxSize);
      memcpy(sSystemStackSnapshot, sSystemStackSnapshotAddress, sSystemStackSnapshotSize);
    }

    // Store the interruption status cortex register
    if (setjmp(*regsBufferAddress)) {
      /* WARNING: at this point the stack is corrupted. You can't use variable
       * stored on the stack. To prevent the compiler to do so, we ensure to
       * user global variables only which aren't stored on the stack. */
      // Restore stack frame
      if (sCheckpointType == CheckpointType::Home) {
        memcpy(sHomeStackSnapshotAddress, sHomeStackSnapshot, sHomeStackSnapshotSize);
      } else if (sCheckpointType == CheckpointType::User) {
        memcpy(sUserStackSnapshotAddress, sUserStackSnapshot, sUserStackSnapshotSize);
      } else {
        assert(sCheckpointType == CheckpointType::System);
        memcpy(sSystemStackSnapshotAddress, sSystemStackSnapshot, sSystemStackSnapshotSize);
      }
    }
  }
  if (sInternalStatus == InternalStatus::PendingLoadCheckpoint) {
    sInternalStatus = InternalStatus::Loaded;
  } else {
    assert(sInternalStatus == InternalStatus::PendingSetCheckpoint);
    sInternalStatus = InternalStatus::Set;
  }
}
