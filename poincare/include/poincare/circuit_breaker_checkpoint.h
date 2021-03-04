#ifndef POINCARE_CIRCUIT_BREAKER_CHECKPOINT_H
#define POINCARE_CIRCUIT_BREAKER_CHECKPOINT_H

#include <ion/circuit_breaker.h>
#include "tree_node.h"
#include "tree_pool.h"

/* Usage:
 *
 * CAUTION : A scope MUST be created directly around the
 * CircuitBreakerCheckpoint, to ensure make the code afterwards
 * non-interruptable. Indeed, the scope calls the checkpoint destructor,
 * which unset the checkpoint.
 *
 void interruptableCode() {
 {
   Poincare::CircuitBreakerCheckpoint checkpoint;
   if (!checkpoint.didInterrupt()) {
     CodeInvolvingLongComputations();
   } else {
     InterruptionHandler();
    }
  }

To manually interrupt : CircuitBreakerCheckpoint::InterruptDueToReductionFailure();

*/

#define CircuitBreakerRun(checkpoint, overridePreviousCheckpoint) (checkpoint.setActive(Ion::CircuitBreaker::setCustomCheckpoint(overridePreviousCheckpoint)))

namespace Poincare {

class CircuitBreakerCheckpoint final {
public:
  static void InterruptDueToReductionFailure();

  CircuitBreakerCheckpoint();
  ~CircuitBreakerCheckpoint();

  bool setActive(Ion::CircuitBreaker::Status status);
  void reset();

private:
  static CircuitBreakerCheckpoint * s_currentCircuitBreakerCheckpoint;

  TreeNode * m_endOfPoolBeforeCheckpoint;
};

}

#endif

