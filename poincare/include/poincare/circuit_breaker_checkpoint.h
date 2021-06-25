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
  void reset();
  virtual Ion::CircuitBreaker::CheckpointType type() const = 0;
private:
  virtual void setCurrentCircuitBreakerCheckpoint(CircuitBreakerCheckpoint *) = 0;
  virtual bool isCurrentCircuitBreakerCheckpoint() const = 0;
};

class UserCircuitBreakerCheckpoint final : public CircuitBreakerCheckpoint {
public:
  using CircuitBreakerCheckpoint::CircuitBreakerCheckpoint;
  ~UserCircuitBreakerCheckpoint();
  Ion::CircuitBreaker::CheckpointType type() const override { return Ion::CircuitBreaker::CheckpointType::User; }
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
  using CircuitBreakerCheckpoint::CircuitBreakerCheckpoint;
  ~SystemCircuitBreakerCheckpoint();
  Ion::CircuitBreaker::CheckpointType type() const override { return Ion::CircuitBreaker::CheckpointType::System; }
private:
  void setCurrentCircuitBreakerCheckpoint(CircuitBreakerCheckpoint * checkpoint) override { s_currentSystemCircuitBreakerCheckpoint = checkpoint; }
  virtual bool isCurrentCircuitBreakerCheckpoint() const override { return s_currentSystemCircuitBreakerCheckpoint == this; }
  static CircuitBreakerCheckpoint * s_currentSystemCircuitBreakerCheckpoint;
};

}

#endif

