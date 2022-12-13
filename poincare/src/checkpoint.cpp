#include <poincare/checkpoint.h>

namespace Poincare {

Checkpoint * Checkpoint::s_topmost = nullptr;

void Checkpoint::protectedDiscard() const {
  if (s_topmost == this) {
    s_topmost = m_parent;
  }
}

void Checkpoint::rollbackException() {
  assert(s_topmost == this);
  discard();
  m_parent->rollbackException();
}

}
