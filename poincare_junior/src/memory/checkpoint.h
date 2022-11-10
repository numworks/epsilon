#ifndef POINCARE_MEMORY_CHECKPOINT_H
#define POINCARE_MEMORY_CHECKPOINT_H

#include "edition_pool.h"

/* Usage:
 *
 * CAUTION : A scope MUST be created directly around the Checkpoint, to ensure
 * to forget the Checkpoint once the interruptable code is executed. Indeed,
 * the scope calls the checkpoint destructor, which invalidate the current
 * checkpoint.
 * Also, any node stored under TopmostEndOfPoolBeforeCheckpoint should not be
 * altered.

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
protected:
  virtual void rollback() { EditionPool::sharedEditionPool()->flush(); }
};

}

#endif

