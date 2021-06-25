#include <poincare/circuit_breaker_checkpoint.h>
#include <poincare/expression.h>
#include <ion.h>

namespace Poincare {

CircuitBreakerCheckpoint * UserCircuitBreakerCheckpoint::s_currentUserCircuitBreakerCheckpoint;
CircuitBreakerCheckpoint * SystemCircuitBreakerCheckpoint::s_currentSystemCircuitBreakerCheckpoint;

bool CircuitBreakerCheckpoint::setActive(Ion::CircuitBreaker::Status status) {
  switch (status) {
    case Ion::CircuitBreaker::Status::Ignored:
      return true;
    case Ion::CircuitBreaker::Status::Set:
      setCurrentCircuitBreakerCheckpoint(this);
      return true;
    case Ion::CircuitBreaker::Status::Interrupted:
      rollback();
      return false;
    default:
      assert(false);
      return false;
  };
}

void CircuitBreakerCheckpoint::reset() {
  if (isCurrentCircuitBreakerCheckpoint()) {
    Ion::CircuitBreaker::unsetCheckpoint(type());
    setCurrentCircuitBreakerCheckpoint(nullptr);
  }
}

UserCircuitBreakerCheckpoint::~UserCircuitBreakerCheckpoint() {
  reset();
}

SystemCircuitBreakerCheckpoint::~SystemCircuitBreakerCheckpoint() {
  reset();
}

#if __EMSCRIPTEN__
bool sInterrupted = false;

bool SystemCircuitBreakerCheckpoint::HasBeenInterrupted() {
  return sInterrupted;
}

void SystemCircuitBreakerCheckpoint::ClearInterruption() {
  sInterrupted = false;
}

void SystemCircuitBreakerCheckpoint::InterruptDueToReductionFailure() {
  sInterrupted = true;
}
#else
void SystemCircuitBreakerCheckpoint::InterruptDueToReductionFailure() {
  if (Ion::CircuitBreaker::hasCheckpoint(Ion::CircuitBreaker::CheckpointType::System)) {
    Ion::CircuitBreaker::loadCheckpoint(Ion::CircuitBreaker::CheckpointType::System);
  }
}
#endif

}
