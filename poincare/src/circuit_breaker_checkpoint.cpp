#include <poincare/circuit_breaker_checkpoint.h>
#include <poincare/expression.h>
#include <ion.h>

namespace Poincare {

CircuitBreakerCheckpoint * CircuitBreakerCheckpoint::s_currentCircuitBreakerCheckpoint;

void CircuitBreakerCheckpoint::InterruptDueToReductionFailure() {
  Expression::ReductionFailed();
  Ion::CircuitBreaker::loadCustomCheckpoint();
}

CircuitBreakerCheckpoint::~CircuitBreakerCheckpoint() {
  reset();
}

bool CircuitBreakerCheckpoint::setActive(Ion::CircuitBreaker::Status status) {
  switch (status) {
    case Ion::CircuitBreaker::Status::Ignored:
      return true;
    case Ion::CircuitBreaker::Status::Set:
      s_currentCircuitBreakerCheckpoint = this;
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
  if (s_currentCircuitBreakerCheckpoint == this) {
    Ion::CircuitBreaker::unsetCustomCheckpoint();
    s_currentCircuitBreakerCheckpoint = nullptr;
  }
}

}
