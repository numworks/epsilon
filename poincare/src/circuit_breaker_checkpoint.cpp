#include <poincare/circuit_breaker_checkpoint.h>
#include <poincare/expression.h>
#include <ion.h>

namespace Poincare {

CircuitBreakerCheckpoint * CircuitBreakerCheckpoint::s_currentCircuitBreakerCheckpoint;

void CircuitBreakerCheckpoint::InterruptDueToReductionFailure() {
  Expression::ReductionFailed();
  Ion::CircuitBreaker::loadCustomCheckpoint();
}

CircuitBreakerCheckpoint::CircuitBreakerCheckpoint() :
  m_endOfPoolBeforeCheckpoint(TreePool::sharedPool()->last())
{
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
      Poincare::TreePool::sharedPool()->freePoolFromNode(m_endOfPoolBeforeCheckpoint);
      return false;
    default:
      assert(false);
      return false;
  };
}

void CircuitBreakerCheckpoint::reset() {
  if (s_currentCircuitBreakerCheckpoint == this) {
    Ion::CircuitBreaker::resetCustomCheckpoint();
    s_currentCircuitBreakerCheckpoint = nullptr;
  }
}

}

/*TODO:
1. Kernel setCustomCheckpoin returns true/false to know if it's the currentCheckpoint
2. Inherit: Checkpoint, ExceptionCheckpoint, CircuitBreakerCheckpoint
3.*/
