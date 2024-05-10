#ifndef POINCARE_CIRCUIT_BREAKER_CHECKPOINT
#define POINCARE_CIRCUIT_BREAKER_CHECKPOINT

#include <ion/circuit_breaker.h>

#include "pool_checkpoint.h"

#define CircuitBreakerRun(checkpoint) \
  (CheckpointRun(checkpoint,          \
                 Ion::CircuitBreaker::setCheckpoint(checkpoint.otype())))

namespace Poincare {

class CircuitBreakerCheckpoint final : public PoolCheckpoint {
 public:
  CircuitBreakerCheckpoint(Ion::CircuitBreaker::CheckpointType type)
      : m_type(type) {}
  /* The desctructor will call ~PoolCheckpoint, and thus
   * PoolCheckpoint::discard(), so we call unset instead of discard. */
  virtual ~CircuitBreakerCheckpoint() { unset(); }

  Ion::CircuitBreaker::CheckpointType otype() const { return m_type; }
  bool setActive(Ion::CircuitBreaker::Status status);
  void discard() const override {
    unset();
    PoolCheckpoint::discard();
  }

 private:
  void rollbackCircuitBreaker();
  void unset() const { Ion::CircuitBreaker::unsetCheckpoint(m_type); }

  Ion::CircuitBreaker::CheckpointType m_type;
};

}  // namespace Poincare

#endif
