#include <drivers/circuit_breaker.h>
#include <drivers/cache.h>
#include <kernel/boot/isr.h>
#include <regs/regs.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

enum class Action {
  Store,
  Load
};

Action sNextAction;
/* Basic frame keeps up to 0x24 bytes */
static constexpr size_t k_basicFrameSize = 0x20;
static constexpr size_t k_extendedFrameSize = 0x68;
uint8_t sFrame[k_extendedFrameSize];
uint32_t sExcReturn = 0;
bool sLock = false;

size_t frameSize(uint32_t excReturn) {
  bool useBasicFrame = excReturn & 0x10;
  return useBasicFrame ? k_basicFrameSize : k_extendedFrameSize;
}

void pendsv_handler(uint8_t * frameAddress, uint32_t excReturn) {
  // Disable other exceptions or change priority!
  /* Step 1: get exception return value indicating the return context:
   * - the stack frame used (FPU extended?),
   * - the stack pointer used (MSP/PSP)
   * - the mode used (thread/handler) */

  size_t frSize = frameSize(excReturn);

  /*NOTE: On exception entry, the ARM hardware uses bit 9 of the stacked xPSR value to indicate whether 4 bytes of padding was added to align the stack on an 8 byte boundary.
   * using the STKALIGN bit of the Configuration Control Register (CCR).
   * In the Cortex-M7 processor CCR.STKALIGN is read-only and has a value of 1. This means that the exception stack frame starting address is always 8-byte aligned.
   * assert CCR.STKALIGN == 1?*/
  // TODO: alignement issue!!!

  if (sNextAction == Action::Store) {
    sExcReturn = excReturn;
    memcpy(sFrame, frameAddress, frSize);
  } else {
    assert(sNextAction == Action::Load);
    assert(sExcReturn != 0);
    size_t loadedFrSize = frameSize(sExcReturn);
    frameAddress += frSize - loadedFrSize;
    memcpy(frameAddress, sFrame, loadedFrSize);
    sExcReturn = 0;
  }
  sLock = false;
}

namespace Ion {
namespace Device {
namespace Checkpoint {

// Rename circuit breaker?
// Address of the last element
// TODO drawing

bool hasCheckpoint() {
  return sExcReturn != 0;
}

bool setCheckpoint() {
  if (hasCheckpoint()) {
    // Keep the oldest checkpoint
    return false;
  }
  sNextAction = Action::Store;
  sLock = true;
  Ion::Device::Regs::CORTEX.ICSR()->setPENDSVSET(true);
  while (sLock) {}
  return !hasCheckpoint();
}

void unsetCheckpoint() {
  sExcReturn = 0;
}

void loadCheckpoint() {
  sNextAction = Action::Load;
  sLock = true;
  Ion::Device::Regs::CORTEX.ICSR()->setPENDSVSET(true);
  while (sLock) {}
}

}
}
}
