#include <assert.h>
#include <poincare/pool.h>
#include <poincare/pool_checkpoint.h>
#include <poincare/pool_object.h>
#include <poincare/src/memory/tree_stack.h>

namespace Poincare {

PoolCheckpoint* PoolCheckpoint::s_topmost = nullptr;

PoolCheckpoint::PoolCheckpoint()
    : m_parent(s_topmost), m_endOfPool(Pool::sharedPool->last()) {
  assert(!m_parent || m_endOfPool >= m_parent->m_endOfPool);
  assert(Internal::TreeStack::SharedTreeStack->size() == 0);
}

PoolCheckpoint::~PoolCheckpoint() {
  assert(Poincare::Internal::TreeStack::SharedTreeStack->size() == 0);
  protectedDiscard();
}

void PoolCheckpoint::protectedDiscard() const {
  if (s_topmost == this) {
    s_topmost = m_parent;
  }
}

void PoolCheckpoint::rollback() const {
  /* NOTE: A rollback may be triggered when:
   * - Pressing Home in an external app or a python environment
   * - Other Home press, or CircuitBreaker
   * - On a PoolCheckpoint::Raise
   *
   * In the first case the data contained in the object may be corrupted but
   * the shared object should be uninitialised anyway. */
  if (Internal::SharedTreeStack.isInitialized()) {
    Internal::SharedTreeStack->flush();
  } else {
    Internal::SharedTreeStack.init();
  }
  if (Pool::sharedPool.isInitialized()) {
    Pool::sharedPool->freePoolFromObject(m_endOfPool);
  } else {
    Pool::sharedPool.init();
  }
}

void PoolCheckpoint::rollbackException() {
  assert(s_topmost == this);
  discard();
  m_parent->rollbackException();
}

}  // namespace Poincare
