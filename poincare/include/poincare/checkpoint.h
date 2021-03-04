#ifndef POINCARE_CHECKPOINT_H
#define POINCARE_CHECKPOINT_H

#include "tree_pool.h"
#include "tree_node.h"

/* Usage:
 *
 * CAUTION : A scope MUST be created directly around the Checkpoint, to ensure
 * to forget the Checkpoint once the interruptable code is executed. Indeed,
 * the scope calls the checkpoint destructor, which invalidate the current
 * checkpoint.

void interruptableCode() {
  Poincare::Checkpoint cp;
  if (CheckpointRun(cp)) {
    CodeInvolvingLongComputationsOrLargeMemoryNeed();
  } else {
    ErrorHandler();
  }
}

*/

#define CheckpointRun(checkpoint, activation) (checkpoint.setActive(activation))

namespace Poincare {

class Checkpoint {
public:
  Checkpoint() : m_endOfPoolBeforeCheckpoint(TreePool::sharedPool()->last()) {}
protected:
  virtual void rollback() { Poincare::TreePool::sharedPool()->freePoolFromNode(m_endOfPoolBeforeCheckpoint); }
private:
  TreeNode * m_endOfPoolBeforeCheckpoint;
};

}

#endif
