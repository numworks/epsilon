#include <assert.h>
#include <poincare/pool.h>
#include <poincare/pool_checkpoint.h>
#include <poincare/pool_object.h>

namespace Poincare {

PoolCheckpoint* PoolCheckpoint::s_topmost = nullptr;

PoolCheckpoint::PoolCheckpoint()
    : m_parent(s_topmost), m_endOfPool(Pool::sharedPool->last()) {
  assert(!m_parent || m_endOfPool >= m_parent->m_endOfPool);
  assert(Internal::TreeStack::SharedTreeStack->size() == 0);
}

void PoolCheckpoint::protectedDiscard() const {
  if (s_topmost == this) {
    s_topmost = m_parent;
  }
}

void PoolCheckpoint::rollback() const {
  Internal::TreeStack::SharedTreeStack->flush();
  Pool::sharedPool->freePoolFromObject(m_endOfPool);
}

void PoolCheckpoint::rollbackException() {
  assert(s_topmost == this);
  discard();
  m_parent->rollbackException();
}

}  // namespace Poincare
