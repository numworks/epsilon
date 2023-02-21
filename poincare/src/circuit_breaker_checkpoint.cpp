#include <poincare/circuit_breaker_checkpoint.h>

namespace Poincare {

bool CircuitBreakerCheckpoint::setActive(Ion::CircuitBreaker::Status status) {
  switch (status) {
    case Ion::CircuitBreaker::Status::Ignored:
      return true;

    case Ion::CircuitBreaker::Status::Set:
      assert(s_topmost == m_parent);
      s_topmost = this;
      return true;

    default:
      assert(status == Ion::CircuitBreaker::Status::Interrupted);
      rollbackCircuitBreaker();
      return false;
  }
}

void CircuitBreakerCheckpoint::rollbackCircuitBreaker() {
  /* The circuit breaker should have unset all lower types of checkpoints when
   * jumping. */
  assert(!Ion::CircuitBreaker::hasCheckpoint(
      static_cast<Ion::CircuitBreaker::CheckpointType>(
          static_cast<uint8_t>(type()) + 1)));
  /* At this point, checkpoints after this can be located in the unwound stack.
   * We must not call their methods. */
  s_topmost = this;
  rollback();
}

}  // namespace Poincare
