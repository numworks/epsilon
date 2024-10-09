#include <assert.h>
#include <poincare/old/pool.h>
#include <poincare/old/pool_checkpoint.h>
#include <poincare/old/pool_object.h>
#include <poincare/src/memory/tree_stack.h>

namespace Poincare {

PoolCheckpoint* PoolCheckpoint::s_topmost = nullptr;

PoolCheckpoint::PoolCheckpoint()
    : m_parent(s_topmost), m_endOfPool(Pool::sharedPool->last()) {
  assert(!m_parent || m_endOfPool >= m_parent->m_endOfPool);
}

void PoolCheckpoint::protectedDiscard() const {
  if (s_topmost == this) {
    s_topmost = m_parent;
  }
}

void PoolCheckpoint::rollback() const {
  Internal::TreeStack::SharedTreeStack->flush();
  Pool::sharedPool->freePoolFromNode(m_endOfPool);
}

void PoolCheckpoint::rollbackException() {
  assert(s_topmost == this);
  discard();
  m_parent->rollbackException();
}

}  // namespace Poincare
