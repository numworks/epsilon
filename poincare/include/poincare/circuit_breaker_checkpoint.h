#ifndef POINCARE_CIRCUIT_BREAKER_CHECKPOINT_H
#define POINCARE_CIRCUIT_BREAKER_CHECKPOINT_H

#include <ion/circuit_breaker.h>
#include <poincare/checkpoint.h>

/* Usage: See comment in checkpoint.h
 *
 * To manually interrupt : CircuitBreakerCheckpoint::InterruptDueToReductionFailure();
 *
 */

#define CircuitBreakerRun(checkpoint, overridePreviousCheckpoint) (CheckpointRun(checkpoint, Ion::CircuitBreaker::setCustomCheckpoint(overridePreviousCheckpoint)))

namespace Poincare {

class CircuitBreakerCheckpoint final : public Checkpoint {
public:
  static void InterruptDueToReductionFailure();

  using Checkpoint::Checkpoint;
  ~CircuitBreakerCheckpoint();

  bool setActive(Ion::CircuitBreaker::Status status);
  void reset();
private:
  static CircuitBreakerCheckpoint * s_currentCircuitBreakerCheckpoint;
};

}

#endif

