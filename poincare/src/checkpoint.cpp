#include <assert.h>
#include <poincare/checkpoint.h>
#include <poincare/tree_node.h>
#include <poincare/tree_pool.h>

namespace Poincare {

Checkpoint* Checkpoint::s_topmost = nullptr;

Checkpoint::Checkpoint()
    : m_parent(s_topmost), m_endOfPool(Pool::sharedPool->last()) {
  assert(!m_parent || m_endOfPool >= m_parent->m_endOfPool);
}

void Checkpoint::protectedDiscard() const {
  if (s_topmost == this) {
    s_topmost = m_parent;
  }
}

void Checkpoint::rollback() const {
  Pool::sharedPool->freePoolFromNode(m_endOfPool);
}

void Checkpoint::rollbackException() {
  assert(s_topmost == this);
  discard();
  m_parent->rollbackException();
}

}  // namespace Poincare
