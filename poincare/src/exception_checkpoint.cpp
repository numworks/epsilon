#include <assert.h>
#include <poincare/exception_checkpoint.h>

namespace Poincare {

void ExceptionCheckpoint::Raise() {
  assert(s_topmost != nullptr);
#if ASSERTIONS
  char temp;
#endif
  /* This assert triggers when we plan to longjmp but the topmost checkpoint
   * should actually already be discarded as it's not located in the active
   * stack. Instead of jumping backward in the stack, we are about to jump
   * forward, which is not allowed.
   *
   *  stack growth direction --->
   *
   * ||------ ACTIVE STACK -------||--- GARBAGE ---
   * | ...back trace... |  Raise  ||
   *                        v              v
   *                      &temp    >    s_topmost
   *
   * &temp is in the currect stackframe: valid address
   *
   * Since the stack grows from higher addresses to lower address, if s_topmost
   * is smaller (ie s_topmost < &temp), it means it's located in the garbage:
   * invalid address
   *
   */
  assert((void*)&temp < (void*)s_topmost);
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
  assert(Poincare::Internal::TreeStack::SharedTreeStack->size() == 0);
  longjmp(m_jumpBuffer, 1);
}

}  // namespace Poincare
