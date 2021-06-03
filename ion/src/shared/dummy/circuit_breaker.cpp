#include <ion/circuit_breaker.h>

namespace Ion {
namespace CircuitBreaker {

Status status() {
  return Status::Busy;
}

bool hasCheckpoint(CheckpointType type) {
  return false;
}

void unsetCheckpoint(CheckpointType type) {}

void loadCheckpoint(CheckpointType type) {}

Status setCheckpoint(CheckpointType type) {
  return Status::Ignored;
}

}
}
