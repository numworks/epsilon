#ifndef POINCARE_CIRCUIT_BREAKER_CHECKPOINT_H
#define POINCARE_CIRCUIT_BREAKER_CHECKPOINT_H

#include <ion/circuit_breaker.h>
#include <poincare/checkpoint.h>

/* Usage: See comment in checkpoint.h
 *
 * To manually interrupt : CircuitBreakerCheckpoint::InterruptDueToReductionFailure();
 *
 */

#define BackCircuitBreakerRun(checkpoint) (CheckpointRun(checkpoint, Ion::CircuitBreaker::setCheckpoint(Ion::CircuitBreaker::CheckpointType::Back)))

#define AnyKeyCircuitBreakerRun(checkpoint) (CheckpointRun(checkpoint, Ion::CircuitBreaker::setCheckpoint(Ion::CircuitBreaker::CheckpointType::AnyKey)))

namespace Poincare {

class UserCircuitBreakerCheckpoint : public Checkpoint {
public:
  using Checkpoint::Checkpoint;
  ~UserCircuitBreakerCheckpoint();
  bool setActive(Ion::CircuitBreaker::Status status);
  void discard();
  static TreeNode * TopmostEndOfPoolBeforeCheckpoint() { return s_currentUserCircuitBreakerCheckpoint ? s_currentUserCircuitBreakerCheckpoint->getEndOfPoolBeforeCheckpoint() : nullptr; }
  static void DiscardYoungerCircuitBreakerCheckpoints(TreeNode * node) { if (node < TopmostEndOfPoolBeforeCheckpoint()) { s_currentUserCircuitBreakerCheckpoint->discard(); } }
private:
  void setCurrentCircuitBreakerCheckpoint(UserCircuitBreakerCheckpoint * checkpoint) { s_currentUserCircuitBreakerCheckpoint = checkpoint; }
  bool isCurrentCircuitBreakerCheckpoint() const { return s_currentUserCircuitBreakerCheckpoint == this; }
  /* Keep current s_topmostExceptionCheckpoint to be able to restore it in a
   * valid state if CircuitBreakerCheckpoint is used. */
  static Checkpoint * s_topmostExceptionCheckpoint;
  static UserCircuitBreakerCheckpoint * s_currentUserCircuitBreakerCheckpoint;
};

}

#endif
