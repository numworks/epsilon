#include <poincare/exception_checkpoint.h>

namespace Poincare {

#if __EMSCRIPTEN__

bool sInterrupted = false;

bool ExceptionCheckpoint::HasBeenInterrupted() { return sInterrupted; }

void ExceptionCheckpoint::ClearInterruption() { sInterrupted = false; }

void ExceptionCheckpoint::Raise() { sInterrupted = true; }

#else

void ExceptionCheckpoint::Raise() {
  assert(s_topmost != nullptr);
  s_topmost->rollbackException();
  assert(false);
}

#endif

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
