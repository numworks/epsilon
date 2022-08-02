#include <assert.h>
#include <drivers/svcall.h>
#include <ion/circuit_breaker.h>

namespace Ion {
namespace CircuitBreaker {

bool SVC_ATTRIBUTES hasCheckpoint(CheckpointType type) {
  SVC_RETURNING_R0(SVC_CIRCUIT_BREAKER_HAS_CHECKPOINT, bool)
}

void SVC_ATTRIBUTES loadCheckpoint(CheckpointType type) {
  SVC_RETURNING_VOID(SVC_CIRCUIT_BREAKER_LOAD_CHECKPOINT)
}

Status stallUntilReady() {
  Status s = status();
  while (s == Ion::CircuitBreaker::Status::Busy) {
    s = status();
  }
  assert(s == Status::Set || s == Status::Interrupted);
  return s;
}

bool SVC_ATTRIBUTES kernelSetCheckpoint(CheckpointType type) {
  SVC_RETURNING_R0(SVC_CIRCUIT_BREAKER_SET_CHECKPOINT, bool)
}

Status setCheckpoint(CheckpointType type) {
  bool checkpointHasBeenSet = kernelSetCheckpoint(type);
  if (!checkpointHasBeenSet) {
    return Status::Ignored;
  }
  return stallUntilReady();
}

Ion::CircuitBreaker::Status SVC_ATTRIBUTES status() {
  SVC_RETURNING_R0(SVC_CIRCUIT_BREAKER_STATUS, Ion::CircuitBreaker::Status)
}

void SVC_ATTRIBUTES unsetCheckpoint(CheckpointType type) {
  SVC_RETURNING_VOID(SVC_CIRCUIT_BREAKER_UNSET_CHECKPOINT)
}

void SVC_ATTRIBUTES lock() {
  SVC_RETURNING_VOID(SVC_CIRCUIT_BREAKER_LOCK)
}

void SVC_ATTRIBUTES unlock() {
  SVC_RETURNING_VOID(SVC_CIRCUIT_BREAKER_UNLOCK)
}

}
}
