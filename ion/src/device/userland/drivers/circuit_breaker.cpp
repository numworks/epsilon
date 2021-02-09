#include <ion/circuit_breaker.h>
#include <userland/drivers/svcall.h>

namespace Ion {
namespace CircuitBreaker {

void SVC_ATTRIBUTES hasCheckpointSVC(bool * res) {
  SVC(SVC_CIRCUIT_BREAKER_HAS_CHECKPOINT);
}

bool hasCheckpoint() {
  bool res;
  hasCheckpointSVC(&res);
  return res;
}

void __attribute__((naked)) loadCheckpoint() {
  SVC(SVC_CIRCUIT_BREAKER_LOAD_CHECKPOINT);
  assert(false);
}

void __attribute__((naked)) setCheckpoint() {
  SVC(SVC_CIRCUIT_BREAKER_SET_CHECKPOINT);
  asm("bx lr");
}

void unsetCheckpoint() {
  SVC(SVC_CIRCUIT_BREAKER_UNSET_CHECKPOINT);
}

}
}
