#include <ion/circuit_breaker.h>
#include <assert.h>
#include <userland/drivers/svcall.h>

namespace Ion {
namespace CircuitBreaker {

bool SVC_ATTRIBUTES hasCheckpoint(CheckpointType type) {
  SVC(SVC_CIRCUIT_BREAKER_HAS_CHECKPOINT);
}

void SVC_ATTRIBUTES loadCheckpoint(CheckpointType type) {
  SVC(SVC_CIRCUIT_BREAKER_LOAD_CHECKPOINT);
}

Status stallUntilReady() {
  Status s = status();
  while (s == Ion::CircuitBreaker::Status::Busy) {
    s = status();
  }
  assert(s == Status::Set || s == Status::Interrupted);
  return s;
}

// TODO
bool SVC_ATTRIBUTES kernelSetCheckpoint(CheckpointType type, uint8_t * spAddress) {
  SVC(SVC_CIRCUIT_BREAKER_SET_CHECKPOINT);
}

Status setCheckpoint(CheckpointType type) {
  uint8_t * stackPointerAddress = nullptr;
  asm volatile ("mov %[stackPointer], sp" : [stackPointer] "=r" (stackPointerAddress) :);
  /* We extrat the stack pointer value when calling setCheckpoint in order to
   * know which portion of the stack needs to be reload to jump back at
   * checkpoint. However, the above asm instruction is executed after the
   * function prologue which might store up to 6 registers on the stack. We
   * slide the stack pointer value to take into account the function prologue. */
  stackPointerAddress += 6*4;
  bool checkpointHasBeenSet = kernelSetCheckpoint(type, stackPointerAddress);
  if (!checkpointHasBeenSet) {
    return Status::Ignored;
  }
  return stallUntilReady();
}

Ion::CircuitBreaker::Status SVC_ATTRIBUTES status() {
  SVC(SVC_CIRCUIT_BREAKER_STATUS);
}

void SVC_ATTRIBUTES unsetCheckpoint(CheckpointType type) {
  SVC(SVC_CIRCUIT_BREAKER_UNSET_CHECKPOINT);
}

}
}
