#include <ion/circuit_breaker.h>
#include <assert.h>
#include <userland/drivers/svcall.h>

namespace Ion {
namespace CircuitBreaker {

void SVC_ATTRIBUTES busySVC(bool * res) {
  SVC(SVC_CIRCUIT_BREAKER_BUSY);
}

bool busy() {
  bool res;
  busySVC(&res);
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

void SVC_ATTRIBUTES storeCustomCheckpointSVC(void * buffer) {
  SVC(SVC_CIRCUIT_BREAKER_STORE_CUSTOM_CHECKPOINT);
}

void storeCustomCheckpoint(CheckpointBuffer buffer) {
  storeCustomCheckpointSVC(&buffer);
}

void SVC_ATTRIBUTES resetCustomCheckpointSVC(void * buffer) {
  SVC(SVC_CIRCUIT_BREAKER_RESET_CUSTOM_CHECKPOINT);
}

void resetCustomCheckpoint(CheckpointBuffer * buffer) {
  resetCustomCheckpointSVC(buffer);
}

void setHomeCheckpoint() {
  SVC(SVC_CIRCUIT_BREAKER_SET_HOME_CHECKPOINT);
  while (Ion::CircuitBreaker::busy()) {}
}

void loadCustomCheckpoint() {
  SVC(SVC_CIRCUIT_BREAKER_LOAD_CUSTOM_CHECKPOINT);
}

void SVC_ATTRIBUTES setCustomCheckpointSVC(bool * overridePreviousCheckpoint) {
  SVC(SVC_CIRCUIT_BREAKER_SET_CUSTOM_CHECKPOINT);
  while (Ion::CircuitBreaker::busy()) {}
}

void setCustomCheckpoint(bool overridePreviousCheckpoint) {
  setCustomCheckpointSVC(&overridePreviousCheckpoint);
}

}
}
