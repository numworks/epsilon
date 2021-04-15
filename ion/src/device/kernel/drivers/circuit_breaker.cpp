#include <drivers/circuit_breaker.h>
#include <drivers/cache.h>
#include <kernel/boot/isr.h>
#include <regs/regs.h>
#include <assert.h>
#include <setjmp.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

extern "C" {
  extern const void * _main_stack_start;
}

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

/* Once in jumping to pendsv_handler, the process stack holds the exception
 * context frame but also miscellaneous values that were pushed by the
 * setCheckpoint method of the userland. The main stack only holds values pushed
 * by the kernel pendsv_handler.
 *
 * The process stack looks like this:
 *
 *        |     |
 *        |     |
 *       C+-----+C  <-- Process stack pointer
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
 * The main stack may look like this:
 *        |     |
 *        |     |
 *       D+-----+D  <-- stack pointer stored by setjmp and restore by longjmp
 *        |     |
 *        |     |   <-- Miscellaneous values stored by
 *        |     |       pendsv_handler before executing setjmp
 *        |     |
 *       C+-----+C  <-- Main stack start
 *        |     |
 *        |     |
 *
 * - Details of BC section -
 * Basic context frame description:
 * | r0 | r1 | r2 | r3 | r12 | lr | return address | xPSR | Reserved | Reserved |
 * Extended context frame description:
 * | r0 | r1 | r2 | r3 | r12 | lr | return address | xPSR | s0 | s1 | ... | s 15 | FPSCR | Reserved | Reserved | Reserved |
 *
 * We need to store/restore a stack snapshot from the point A to C and C to D
 * to ensure that right after the longjmp:
 * - the pendsv_handler ends correctly (C-D section)
 * - the pendsv_handler returns to the right method among the userland API
 *   Ion::CircuitBreaker (B-C section)
 * - the Ion::CircuitBreaker userland API returns correclty to the application
 *   code that used the Ion::CircuitBreaker API initially (A-B section)
 *
 * The AB section size is assessed from the extended context frame size (108) +
 * few registers stored on the stack when calling Userland setCheckpoint.
 * The CD section size can be determined dynamically by the pendsv_handler
 * which extracts the stack pointer right before executing setjmp to get the
 * value of D address.
 */

/* Empirically, the snapshot section AC to 144 bytes long in DEBUG=1 (120 bytes
 * long in DEBUG = 0). Keeping a greater snapshot than necessary is not an
 * issue. */
static constexpr size_t k_processStackSnapshotMaxSize = 144;
/* Empirically, the snapshot section CD is up to 48 bytes long in DEBUG = 1, 16
 * bytes long in DEBUG = 0. */
static constexpr size_t k_mainStackSnapshotMaxSize = 48;

enum class InternalStatus {
  Set,
  Loaded,
  PendingSetCheckpoint,
  PendingLoadCheckpoint,
};

InternalStatus sInternalStatus = InternalStatus::Loaded;
CheckpointType sCheckpointType = CheckpointType::Home;

constexpr static int k_numberOfCheckpointTypes = 3;

// Process snapshots
uint8_t sProcessStackSnapshot[k_numberOfCheckpointTypes][k_processStackSnapshotMaxSize];
uint8_t * sProcessStackSnapshotAddress[k_numberOfCheckpointTypes] = {nullptr, nullptr, nullptr};
// Main snapshots
uint8_t sMainStackSnapshot[k_numberOfCheckpointTypes][k_mainStackSnapshotMaxSize];
uint8_t * sMainStackSnapshotAddress[k_numberOfCheckpointTypes] = {nullptr, nullptr, nullptr};
// Registers buffers
jmp_buf sRegisters[k_numberOfCheckpointTypes];

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
  return sProcessStackSnapshotAddress[static_cast<int>(type)] != nullptr;
}

void unsetCheckpoint(CheckpointType type) {
  sProcessStackSnapshotAddress[static_cast<int>(type)] = nullptr;
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

void pendsv_handler() {
  /* We restrain the use of the main stack since we don't take any snapshot of
   * if before a setjmp. */
  {
    // status is supposed to be one of the pending status
    assert(sInternalStatus == InternalStatus::PendingSetCheckpoint || sInternalStatus == InternalStatus::PendingLoadCheckpoint);

    if (sInternalStatus == InternalStatus::PendingLoadCheckpoint) {
      longjmp(sRegisters[static_cast<int>(sCheckpointType)], 1);
    }
  }

  jmp_buf * regsBufferAddress;
  {
    assert(sInternalStatus == InternalStatus::PendingSetCheckpoint);
    int checkpointTypeIndex = static_cast<int>(sCheckpointType);

    // Registers buffer address
    regsBufferAddress = &sRegisters[checkpointTypeIndex];

    // Store main stack snapshot
    uint8_t * mainStackPointer = nullptr;
    asm volatile ("mrs %[stackPointer], msp" : [stackPointer] "=r" (mainStackPointer) :);
    size_t mainSnapshotSize = reinterpret_cast<uint8_t *>(&_main_stack_start) - mainStackPointer;
    assert(mainSnapshotSize <= k_mainStackSnapshotMaxSize);
    memcpy(sMainStackSnapshot[checkpointTypeIndex], mainStackPointer, mainSnapshotSize);
    sMainStackSnapshotAddress[checkpointTypeIndex] = mainStackPointer;

    // Store process stack snapshot
    uint8_t * processStackPointer = nullptr;
    asm volatile ("mrs %[stackPointer], psp" : [stackPointer] "=r" (processStackPointer) :);
    memcpy(sProcessStackSnapshot[checkpointTypeIndex], processStackPointer, k_processStackSnapshotMaxSize);
    sProcessStackSnapshotAddress[checkpointTypeIndex] = processStackPointer;
  }

  // Store the interruption status cortex register
  /* regsBufferAddress is the only variable stored on the main stack. It's
   * alright since its value is not used after longjmp. */
  if (setjmp(*regsBufferAddress)) {
    /* WARNING: at this point the stack is corrupted. You can't use variable
     * stored on the stack. To prevent the compiler to do so, we ensure to
     * user global variables only which aren't stored on the stack. */
    int checkpointTypeIndex = static_cast<int>(sCheckpointType);
    // Restore process stack
    memcpy(sProcessStackSnapshotAddress[checkpointTypeIndex], sProcessStackSnapshot[checkpointTypeIndex], k_processStackSnapshotMaxSize);
    uint8_t * processStackAddress = sProcessStackSnapshotAddress[checkpointTypeIndex];
    asm volatile ("msr psp, %[stackPointer]" : : [stackPointer] "r" (processStackAddress));

    // Restore main stack
    size_t mainSnapshotSize = reinterpret_cast<uint8_t *>(&_main_stack_start) - sMainStackSnapshotAddress[checkpointTypeIndex];
    memcpy(sMainStackSnapshotAddress[checkpointTypeIndex], sMainStackSnapshot[checkpointTypeIndex], mainSnapshotSize);

  }

  if (sInternalStatus == InternalStatus::PendingLoadCheckpoint) {
    sInternalStatus = InternalStatus::Loaded;
  } else {
    assert(sInternalStatus == InternalStatus::PendingSetCheckpoint);
    sInternalStatus = InternalStatus::Set;
  }
}
