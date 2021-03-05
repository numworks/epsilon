#include <ion/circuit_breaker.h>
#include <assert.h>
#include <userland/drivers/svcall.h>

namespace Ion {
namespace CircuitBreaker {

void SVC_ATTRIBUTES hasCheckpointSVC(CheckpointType * type, bool * res) {
  SVC(SVC_CIRCUIT_BREAKER_HAS_CHECKPOINT);
}

bool hasCheckpoint(CheckpointType type) {
  bool res;
  hasCheckpointSVC(&type, &res);
  return res;
}

void SVC_ATTRIBUTES loadCheckpointSVC(CheckpointType * type) {
  SVC(SVC_CIRCUIT_BREAKER_LOAD_CHECKPOINT);
}

void loadCheckpoint(CheckpointType type) {
  loadCheckpointSVC(&type);
}

void SVC_ATTRIBUTES setCheckpointSVC(CheckpointType * type, bool * checkpointHasBeenSet) {
  SVC(SVC_CIRCUIT_BREAKER_SET_CHECKPOINT);
}

Status stallUntilReady() {
  Status s = status();
  while (s == Ion::CircuitBreaker::Status::Busy) {
    s = status();
  }
  assert(s == Status::Set || s == Status::Interrupted);
  return s;
}

Status setCheckpoint(CheckpointType type) {
  bool checkpointHasBeenSet;
  setCheckpointSVC(&type, &checkpointHasBeenSet);
  if (!checkpointHasBeenSet) {
    return Status::Ignored;
  }
  return stallUntilReady();
}

void SVC_ATTRIBUTES statusSVC(Ion::CircuitBreaker::Status * res) {
  SVC(SVC_CIRCUIT_BREAKER_STATUS);
}

Ion::CircuitBreaker::Status status() {
  Ion::CircuitBreaker::Status res;
  statusSVC(&res);
  return res;
}

void SVC_ATTRIBUTES unsetCheckpointSVC(CheckpointType * type) {
  SVC(SVC_CIRCUIT_BREAKER_UNSET_CHECKPOINT);
}

void unsetCheckpoint(CheckpointType type) {
  unsetCheckpointSVC(&type);
}

}
}
