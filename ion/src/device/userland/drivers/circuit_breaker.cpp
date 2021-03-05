#include <ion/circuit_breaker.h>
#include <assert.h>
#include <userland/drivers/svcall.h>

namespace Ion {
namespace CircuitBreaker {

void SVC_ATTRIBUTES statusSVC(Ion::CircuitBreaker::Status * res) {
  SVC(SVC_CIRCUIT_BREAKER_STATUS);
}

Ion::CircuitBreaker::Status status() {
  Ion::CircuitBreaker::Status res;
  statusSVC(&res);
  return res;
}

void SVC_ATTRIBUTES hasCustomCheckpointSVC(bool * res) {
  SVC(SVC_CIRCUIT_BREAKER_HAS_CUSTOM_CHECKPOINT);
}

bool hasCustomCheckpoint() {
  bool res;
  hasCustomCheckpointSVC(&res);
  return res;
}

void SVC_ATTRIBUTES storeCustomCheckpointSVC(void * buffer) {
  SVC(SVC_CIRCUIT_BREAKER_STORE_CUSTOM_CHECKPOINT);
}

void storeCustomCheckpoint(CheckpointBuffer buffer) {
  storeCustomCheckpointSVC(buffer);
}

void SVC_ATTRIBUTES resetCustomCheckpointSVC(void * buffer) {
  SVC(SVC_CIRCUIT_BREAKER_RESET_CUSTOM_CHECKPOINT);
}

void resetCustomCheckpoint(CheckpointBuffer buffer) {
  resetCustomCheckpointSVC(buffer);
}

void unsetCustomCheckpoint() {
  SVC(SVC_CIRCUIT_BREAKER_UNSET_CUSTOM_CHECKPOINT);
}

void loadCustomCheckpoint() {
  SVC(SVC_CIRCUIT_BREAKER_LOAD_CUSTOM_CHECKPOINT);
}

void SVC_ATTRIBUTES setCustomCheckpointSVC(bool * overridePreviousCheckpoint, bool * checkpointSet) {
  SVC(SVC_CIRCUIT_BREAKER_SET_CUSTOM_CHECKPOINT);
}

Status stallUntilReady() {
  Status s = status();
  while (s == Ion::CircuitBreaker::Status::Busy) {
    s = status();
  }
  assert(s == Status::Set || s == Status::Interrupted);
  return s;
}

Status setCustomCheckpoint(bool overridePreviousCheckpoint) {
  bool checkpointSet;
  setCustomCheckpointSVC(&overridePreviousCheckpoint, &checkpointSet);
  if (!checkpointSet) {
    return Status::Ignored;
  }
  return stallUntilReady();
}

Status setHomeCheckpoint() {
  SVC(SVC_CIRCUIT_BREAKER_SET_HOME_CHECKPOINT);
  return stallUntilReady();
}

}
}
