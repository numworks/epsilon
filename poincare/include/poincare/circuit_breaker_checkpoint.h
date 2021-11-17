#ifndef POINCARE_CIRCUIT_BREAKER_CHECKPOINT_H
#define POINCARE_CIRCUIT_BREAKER_CHECKPOINT_H

#include <ion/circuit_breaker.h>
#include <poincare/checkpoint.h>

/* Usage: See comment in checkpoint.h
 *
 * To manually interrupt : CircuitBreakerCheckpoint::InterruptDueToReductionFailure();
 *
 */

#define CircuitBreakerRun(checkpoint) (CheckpointRun(checkpoint, Ion::CircuitBreaker::setCheckpoint(checkpoint.type())))

namespace Poincare {

class CircuitBreakerCheckpoint : public Checkpoint {
public:
  using Checkpoint::Checkpoint;
  bool setActive(Ion::CircuitBreaker::Status status);
  void discard(CircuitBreakerCheckpoint * checkpoint = nullptr);
  virtual Ion::CircuitBreaker::CheckpointType type() const = 0;
  static TreeNode * TopmostEndOfPoolBeforeCheckpoint();
  static void DiscardYoungerCircuitBreakerCheckpoints(TreeNode * node);
private:
  virtual void setCurrentCircuitBreakerCheckpoint(CircuitBreakerCheckpoint *) = 0;
  virtual bool isCurrentCircuitBreakerCheckpoint() const = 0;
  /* Keep current s_topmostExceptionCheckpoint to be able to restore it in a
   * valid state if CircuitBreakerCheckpoint is used. */
  static Checkpoint * s_topmostExceptionCheckpoint;
};

class UserCircuitBreakerCheckpoint final : public CircuitBreakerCheckpoint {
public:
  using CircuitBreakerCheckpoint::CircuitBreakerCheckpoint;
  ~UserCircuitBreakerCheckpoint();
  Ion::CircuitBreaker::CheckpointType type() const override { return Ion::CircuitBreaker::CheckpointType::User; }
  static TreeNode * TopmostEndOfPoolBeforeCheckpoint() { return s_currentUserCircuitBreakerCheckpoint ? s_currentUserCircuitBreakerCheckpoint->getEndOfPoolBeforeCheckpoint() : nullptr; }
  static void DiscardYoungerCircuitBreakerCheckpoints(TreeNode * node) { if (node < TopmostEndOfPoolBeforeCheckpoint()) { s_currentUserCircuitBreakerCheckpoint->discard(); } }
private:
  void setCurrentCircuitBreakerCheckpoint(CircuitBreakerCheckpoint * checkpoint) override { s_currentUserCircuitBreakerCheckpoint = checkpoint; }
  virtual bool isCurrentCircuitBreakerCheckpoint() const override { return s_currentUserCircuitBreakerCheckpoint == this; }
  static CircuitBreakerCheckpoint * s_currentUserCircuitBreakerCheckpoint;
};

class SystemCircuitBreakerCheckpoint final : public CircuitBreakerCheckpoint {
public:
#if __EMSCRIPTEN__
  static bool HasBeenInterrupted();
  static void ClearInterruption();
#endif
  static void InterruptDueToReductionFailure();
  SystemCircuitBreakerCheckpoint();
  ~SystemCircuitBreakerCheckpoint();
  Ion::CircuitBreaker::CheckpointType type() const override { return Ion::CircuitBreaker::CheckpointType::System; }
  static TreeNode * TopmostEndOfPoolBeforeCheckpoint() { return s_currentSystemCircuitBreakerCheckpoint ? s_currentSystemCircuitBreakerCheckpoint->getEndOfPoolBeforeCheckpoint() : nullptr; }
  static void DiscardYoungerCircuitBreakerCheckpoints(TreeNode * node) { if (node < TopmostEndOfPoolBeforeCheckpoint()) { s_currentSystemCircuitBreakerCheckpoint->discard(); } }
private:
  void setCurrentCircuitBreakerCheckpoint(CircuitBreakerCheckpoint * checkpoint) override { s_currentSystemCircuitBreakerCheckpoint = checkpoint; }
  virtual bool isCurrentCircuitBreakerCheckpoint() const override { return s_currentSystemCircuitBreakerCheckpoint == this; }
  static CircuitBreakerCheckpoint * s_currentSystemCircuitBreakerCheckpoint;
  CircuitBreakerCheckpoint * m_parent;
};

}

#endif

