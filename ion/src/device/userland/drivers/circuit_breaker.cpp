#include <ion/circuit_breaker.h>
#include <assert.h>
#include <userland/drivers/svcall.h>

namespace Ion {
namespace CircuitBreaker {

void SVC_ATTRIBUTES hasCheckpointSVC(CircuitBreaker::Checkpoint * checkpoint, bool * res) {
  SVC(SVC_CIRCUIT_BREAKER_HAS_CHECKPOINT);
}

bool hasCheckpoint(Checkpoint c) {
  bool res;
  hasCheckpointSVC(&c, &res);
  return res;
}

void SVC_ATTRIBUTES unsetCheckpointSVC(CircuitBreaker::Checkpoint * checkpoint) {
  SVC(SVC_CIRCUIT_BREAKER_UNSET_CHECKPOINT);
}

void unsetCheckpoint(Checkpoint c) {
  unsetCheckpointSVC(&c);
}

void __attribute__((naked)) loadHomeCheckpoint() {
  SVC(SVC_CIRCUIT_BREAKER_LOAD_HOME_CHECKPOINT);
  assert(false);
}

void __attribute__((naked)) setHomeCheckpoint() {
  SVC(SVC_CIRCUIT_BREAKER_SET_HOME_CHECKPOINT);
  asm("bx lr");
}

void __attribute__((naked)) loadCustomCheckpoint() {
  SVC(SVC_CIRCUIT_BREAKER_LOAD_CUSTOM_CHECKPOINT);
  assert(false);
}

void __attribute__((naked)) setCustomCheckpoint() {
  SVC(SVC_CIRCUIT_BREAKER_SET_CUSTOM_CHECKPOINT);
  asm("bx lr");
}

}
}
