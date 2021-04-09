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

bool SVC_ATTRIBUTES kernelSetCheckpoint(CheckpointType type) {
  SVC(SVC_CIRCUIT_BREAKER_SET_CHECKPOINT);
}

Status setCheckpoint(CheckpointType type) {
  bool checkpointHasBeenSet = kernelSetCheckpoint(type);
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
