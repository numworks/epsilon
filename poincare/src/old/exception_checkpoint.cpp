#include <assert.h>
#include <poincare/old/exception_checkpoint.h>

namespace Poincare {

void ExceptionCheckpoint::Raise() {
  assert(s_topmost != nullptr);
  s_topmost->rollbackException();
  assert(false);
}

bool ExceptionCheckpoint::setActive(bool interruption) {
  if (!interruption) {
    assert(s_topmost == m_parent);
    s_topmost = this;
  }
  return !interruption;
}

void ExceptionCheckpoint::rollbackException() {
  rollback();
  longjmp(m_jumpBuffer, 1);
}

}  // namespace Poincare
