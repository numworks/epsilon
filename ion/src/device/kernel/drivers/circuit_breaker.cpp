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

/* Context switching has to be done within a pendSV interrupt. Indeed, if the
 * interruption handling the context switching is nested within another
 * interruption, there is no easy way to inform the cortex of the interruption
 * nesting when loading a previous context (since the "active interruption"
 * registers of the cortex are read-only). We use pendSV to delay the context
 * switching until no other interruption remains to be executed. */

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
 * - Details of BC section -
 * Basic context frame description:
 * | r0 | r1 | r2 | r3 | r12 | lr | return address | xPSR | Reserved | Reserved |
 * Extended context frame description:
 * | r0 | r1 | r2 | r3 | r12 | lr | return address | xPSR | s0 | s1 | ... | s 15 | FPSCR | Reserved | Reserved | Reserved |
 *
 * We need to store/restore a stack snapshot from the point A to B to ensure
 * that right after the longjmp:
 * - the pendsv_handler ends correctly (C-D section)
 * - the pendsv_handler returns to the right method among the userland API
 *   Ion::CircuitBreaker (B-C section)
 * - the Ion::CircuitBreaker userland API returns correclty to the application
 *   code that used the Ion::CircuitBreaker API initially (A-B section)
 *
 * The userland setCheckpoint is reponsible for extracting the stack pointer
 * value when entering the fonction to assess the value of A address.
 * The pendsv_handler extracts the stack pointer right before executing setjmp
 * to get the value of D address.
 */

// Empirically, the snapshot section AD is up to 168 bytes long
static constexpr size_t k_stackSnapshotMaxSize = 256;

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

uint8_t * sASectionStart = nullptr;

bool setCheckpoint(CheckpointType type, uint8_t * spAddress) {
  if (Device::CircuitBreaker::hasCheckpoint(type)) {
    // Keep the oldest checkpoint
    return false;
  }
  sASectionStart = spAddress;
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

void pendsv_handler() {
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
      sHomeStackSnapshotSize = sASectionStart - stackPointerAddress;
      assert(sHomeStackSnapshotSize <= k_stackSnapshotMaxSize);
      memcpy(sHomeStackSnapshot, sHomeStackSnapshotAddress, sHomeStackSnapshotSize);
    } else if (sCheckpointType == CheckpointType::User) {
      regsBufferAddress = &sUserRegisters;
      sUserStackSnapshotAddress = stackPointerAddress;
      sUserStackSnapshotSize = sASectionStart - stackPointerAddress;
      assert(sUserStackSnapshotSize <= k_stackSnapshotMaxSize);
      memcpy(sUserStackSnapshot, sUserStackSnapshotAddress, sUserStackSnapshotSize);
    } else {
      assert(sCheckpointType == CheckpointType::System);
      regsBufferAddress = &sSystemRegisters;
      sSystemStackSnapshotAddress = stackPointerAddress;
      sSystemStackSnapshotSize = sASectionStart - stackPointerAddress;
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
