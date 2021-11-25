#include <poincare/checkpoint.h>
#include <poincare/circuit_breaker_checkpoint.h>
#include <poincare/exception_checkpoint.h>

namespace Poincare {

/* Return the topmost end of the pool before last checkpoint. No nodes under
 * it in the pool shall be altered. Return nullptr if there are no checkpoints.
 */
TreeNode * Checkpoint::TopmostEndOfPoolBeforeCheckpoint() {
  TreeNode * exceptionCheckpointEnd = ExceptionCheckpoint::TopmostEndOfPoolBeforeCheckpoint();
  TreeNode * circuitBreakerCheckpointEnd = UserCircuitBreakerCheckpoint::TopmostEndOfPoolBeforeCheckpoint();
  return circuitBreakerCheckpointEnd > exceptionCheckpointEnd ? circuitBreakerCheckpointEnd : exceptionCheckpointEnd;
}

}
