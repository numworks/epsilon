#include <ion/circuit_breaker.h>
#include <assert.h>
#include <userland/drivers/svcall.h>

namespace Ion {
namespace CircuitBreaker {

void SVC_ATTRIBUTES hasCustomCheckpointSVC(bool * res) {
  SVC(SVC_CIRCUIT_BREAKER_HAS_CUSTOM_CHECKPOINT);
}

bool hasCustomCheckpoint() {
  bool res;
  hasCustomCheckpointSVC(&res);
  return res;
}

void SVC_ATTRIBUTES clearCheckpointFlagSVC(bool * isCustomCheckpoint, bool * res) {
  SVC(SVC_CIRCUIT_BREAKER_CLEAR_CHECKPOINT_FLAG);
}

bool clearCustomCheckpointFlag() {
  bool res;
  bool isCustomCheckpoint = true;
  clearCheckpointFlagSVC(&isCustomCheckpoint, &res);
  return res;
}

bool clearHomeCheckpointFlag() {
  bool res;
  bool isCustomCheckpoint = false;
  clearCheckpointFlagSVC(&isCustomCheckpoint, &res);
  return res;
}

void unsetCustomCheckpoint() {
  SVC(SVC_CIRCUIT_BREAKER_UNSET_CUSTOM_CHECKPOINT);
}

void setHomeCheckpoint() {
  SVC(SVC_CIRCUIT_BREAKER_SET_HOME_CHECKPOINT);
}

void loadCustomCheckpoint() {
  SVC(SVC_CIRCUIT_BREAKER_LOAD_CUSTOM_CHECKPOINT);
}

void setCustomCheckpoint() {
  SVC(SVC_CIRCUIT_BREAKER_SET_CUSTOM_CHECKPOINT);
}

}
}
