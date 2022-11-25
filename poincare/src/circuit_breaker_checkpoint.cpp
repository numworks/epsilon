#include <poincare/circuit_breaker_checkpoint.h>
#include <poincare/exception_checkpoint.h>
#include <poincare/expression.h>
#include <ion.h>

namespace Poincare {

Checkpoint * UserCircuitBreakerCheckpoint::s_topmostExceptionCheckpoint;
UserCircuitBreakerCheckpoint * UserCircuitBreakerCheckpoint::s_currentUserCircuitBreakerCheckpoint;

UserCircuitBreakerCheckpoint::~UserCircuitBreakerCheckpoint() {
  discard();
}

bool UserCircuitBreakerCheckpoint::setActive(Ion::CircuitBreaker::Status status) {
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

void UserCircuitBreakerCheckpoint::discard() {
  if (isCurrentCircuitBreakerCheckpoint()) {
    Ion::CircuitBreaker::unsetCheckpoint(Ion::CircuitBreaker::CheckpointType::Back);
    Ion::CircuitBreaker::unsetCheckpoint(Ion::CircuitBreaker::CheckpointType::AnyKey);
    setCurrentCircuitBreakerCheckpoint(nullptr);
  }
}


}
