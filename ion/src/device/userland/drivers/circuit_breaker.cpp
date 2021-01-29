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

void loadCheckpoint() {
  SVC(SVC_CIRCUIT_BREAKER_LOAD_CHECKPOINT);
}

void SVC_ATTRIBUTES setCheckpointSVC(bool * res) {
  SVC(SVC_CIRCUIT_BREAKER_SET_CHECKPOINT);
}

bool setCheckpoint() {
  bool res;
  setCheckpointSVC(&res);
  return res;
}

void unsetCheckpoint() {
  SVC(SVC_CIRCUIT_BREAKER_UNSET_CHECKPOINT);
}

}
}
