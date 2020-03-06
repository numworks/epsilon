#include <poincare/exception_checkpoint.h>

namespace Poincare {

ExceptionCheckpoint * ExceptionCheckpoint::s_topmostExceptionCheckpoint;

ExceptionCheckpoint::ExceptionCheckpoint() :
  m_endOfPoolBeforeCheckpoint(TreePool::sharedPool()->last()),
  m_parent(s_topmostExceptionCheckpoint)
{
  s_topmostExceptionCheckpoint = this;
}

/*
int ExceptionCheckpoint::run() {
  m_endOfPoolBeforeCheckpoint = TreePool::sharedPool()->last();
  m_parent = s_topmostExceptionCheckpoint;
  s_topmostExceptionCheckpoint = this;
  return setjmp(m_jumpBuffer) == 0;
}
*/

void ExceptionCheckpoint::rollback() {
  Poincare::TreePool::sharedPool()->freePoolFromNode(m_endOfPoolBeforeCheckpoint);
  longjmp(m_jumpBuffer, 1);
}

}
