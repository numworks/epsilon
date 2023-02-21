#ifndef POINCARE_CIRCUIT_BREAKER_CHECKPOINT
#define POINCARE_CIRCUIT_BREAKER_CHECKPOINT

#include <ion/circuit_breaker.h>
#include <poincare/checkpoint.h>

#define CircuitBreakerRun(checkpoint) \
  (CheckpointRun(checkpoint,          \
                 Ion::CircuitBreaker::setCheckpoint(checkpoint.type())))

namespace Poincare {

class CircuitBreakerCheckpoint final : public Checkpoint {
 public:
  CircuitBreakerCheckpoint(Ion::CircuitBreaker::CheckpointType type)
      : m_type(type) {}
  /* The desctructor will call ~Checkpoint, and thus Checkpoint::discard(), so
   * we call unset instead of discard. */
  virtual ~CircuitBreakerCheckpoint() { unset(); }

  Ion::CircuitBreaker::CheckpointType type() const { return m_type; }
  bool setActive(Ion::CircuitBreaker::Status status);
  void discard() const override {
    unset();
    Checkpoint::discard();
  }

 private:
  void rollbackCircuitBreaker();
  void unset() const { Ion::CircuitBreaker::unsetCheckpoint(m_type); }

  Ion::CircuitBreaker::CheckpointType m_type;
};

}  // namespace Poincare

#endif
