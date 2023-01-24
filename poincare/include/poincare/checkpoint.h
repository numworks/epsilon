#ifndef POINCARE_CHECKPOINT_H
#define POINCARE_CHECKPOINT_H

#include <poincare/tree_node.h>
#include <poincare/tree_pool.h>

/* Usage:
 *
 * CAUTION : A scope MUST be created directly around the Checkpoint, to ensure
 * to forget the Checkpoint once the interruptable code is executed. Indeed,
 * the scope calls the checkpoint destructor, which invalidate the current
 * checkpoint.
 * Also, any node stored under TopmostEndOfPool should not be altered.

void interruptableCode() {
  Poincare::Checkpoint cp;
  if (CheckpointRun(cp)) {
    CodeInvolvingLongComputationsOrLargeMemoryNeed();
  } else {
    ErrorHandler();
  }
}

*/

#define CheckpointRun(checkpoint, activation) ( checkpoint.setActive(activation) )

namespace Poincare {

class Checkpoint {
  friend class ExceptionCheckpoint;
public:
  static TreeNode * TopmostEndOfPool();

  Checkpoint() : m_parent(s_topmost), m_endOfPool(TreePool::sharedPool->last()) {
    assert(!m_parent || m_endOfPool >= m_parent->m_endOfPool);
  }
  Checkpoint(const Checkpoint &) = delete;
  virtual ~Checkpoint() { protectedDiscard(); }
  Checkpoint & operator=(const Checkpoint &) = delete;

  virtual void discard() const { protectedDiscard(); }

protected:
  static Checkpoint * s_topmost;

  void rollback() const { TreePool::sharedPool->freePoolFromNode(m_endOfPool); }
  void protectedDiscard() const;

  Checkpoint * const m_parent;

private:
  virtual void rollbackException();

  TreeNode * const m_endOfPool;
};

}

#endif
