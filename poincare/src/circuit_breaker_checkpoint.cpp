#include <poincare/circuit_breaker_checkpoint.h>
#include <poincare/exception_checkpoint.h>
#include <poincare/expression.h>
#include <ion.h>

namespace Poincare {

Checkpoint * CircuitBreakerCheckpoint::s_topmostExceptionCheckpoint;
CircuitBreakerCheckpoint * UserCircuitBreakerCheckpoint::s_currentUserCircuitBreakerCheckpoint;
CircuitBreakerCheckpoint * SystemCircuitBreakerCheckpoint::s_currentSystemCircuitBreakerCheckpoint;

bool CircuitBreakerCheckpoint::setActive(Ion::CircuitBreaker::Status status) {
  switch (status) {
    case Ion::CircuitBreaker::Status::Ignored:
      return true;
    case Ion::CircuitBreaker::Status::Set:
      setCurrentCircuitBreakerCheckpoint(this);
      s_topmostExceptionCheckpoint = static_cast<Checkpoint *>(ExceptionCheckpoint::s_topmostExceptionCheckpoint);
      return true;
    case Ion::CircuitBreaker::Status::Interrupted:
      // Restore the topmost exception checkpoint
      ExceptionCheckpoint::s_topmostExceptionCheckpoint = static_cast<ExceptionCheckpoint *>(s_topmostExceptionCheckpoint);
      rollback();
      return false;
    default:
      assert(false);
      return false;
  };
}

void CircuitBreakerCheckpoint::discard(CircuitBreakerCheckpoint * checkpoint) {
  if (isCurrentCircuitBreakerCheckpoint()) {
    Ion::CircuitBreaker::unsetCheckpoint(type());
    setCurrentCircuitBreakerCheckpoint(checkpoint);
  }
}

// Return the topmost end of pool before current circuit breaker checkpoints
TreeNode * CircuitBreakerCheckpoint::TopmostEndOfPoolBeforeCheckpoint() {
  TreeNode * userCircuitBreakerCheckpointEnd = UserCircuitBreakerCheckpoint::TopmostEndOfPoolBeforeCheckpoint();
  TreeNode * systemCircuitBreakerCheckpointEnd = SystemCircuitBreakerCheckpoint::TopmostEndOfPoolBeforeCheckpoint();
  return systemCircuitBreakerCheckpointEnd > userCircuitBreakerCheckpointEnd ? systemCircuitBreakerCheckpointEnd : userCircuitBreakerCheckpointEnd;
}

/* Remove current circuit breaker checkpoints having their topmost end of pool
 * more recent in the stack. */
void CircuitBreakerCheckpoint::DiscardYoungerCircuitBreakerCheckpoints(TreeNode * node) {
  assert(node);
  UserCircuitBreakerCheckpoint::DiscardYoungerCircuitBreakerCheckpoints(node);
  SystemCircuitBreakerCheckpoint::DiscardYoungerCircuitBreakerCheckpoints(node);
}

UserCircuitBreakerCheckpoint::~UserCircuitBreakerCheckpoint() {
  discard();
}

SystemCircuitBreakerCheckpoint::SystemCircuitBreakerCheckpoint() :
  CircuitBreakerCheckpoint(),
  m_parent(s_currentSystemCircuitBreakerCheckpoint)
{
}

SystemCircuitBreakerCheckpoint::~SystemCircuitBreakerCheckpoint() {
  discard(m_parent);
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
