#include "exception_checkpoint.h"

#include <stdlib.h>

#include "tree_stack.h"

namespace PoincareJ {

ExceptionCheckpoint* ExceptionCheckpoint::s_topmostExceptionCheckpoint;
ExceptionType ExceptionCheckpoint::s_exceptionType = ExceptionType::None;

ExceptionCheckpoint::ExceptionCheckpoint(Block* rightmostBlock)
    : m_parent(s_topmostExceptionCheckpoint), m_rightmostBlock(rightmostBlock) {
  assert(s_exceptionType == ExceptionType::None);
}

ExceptionCheckpoint::~ExceptionCheckpoint() {
  assert((s_topmostExceptionCheckpoint == this &&
          s_exceptionType == ExceptionType::None) ||
         (s_topmostExceptionCheckpoint == m_parent &&
          s_exceptionType != ExceptionType::None));
  s_topmostExceptionCheckpoint = m_parent;
}

void ExceptionCheckpoint::rollback() {
  // Next Raise will be handled by parent.
  s_topmostExceptionCheckpoint = m_parent;
  /* Flush everything changed on the SharedTreeStack because it may be
   * corrupted. */
  SharedTreeStack->flushFromBlock(m_rightmostBlock);
  longjmp(m_jumpBuffer, 1);
}

void ExceptionCheckpoint::Raise(ExceptionType type) {
  assert(type != ExceptionType::None && s_exceptionType == ExceptionType::None);
  s_exceptionType = type;
  // Can't raise if there are no active ExceptionCheckpoints.
  if (s_topmostExceptionCheckpoint == nullptr) {
    abort();
  }
  s_topmostExceptionCheckpoint->rollback();
  abort();
}

ExceptionType ExceptionCheckpoint::GetTypeAndClear() {
  ExceptionType type = s_exceptionType;
  s_exceptionType = ExceptionType::None;
  return type;
}

}  // namespace PoincareJ

extern "C" {
void ExceptionCheckpointRaise() {
  PoincareJ::ExceptionCheckpoint::Raise(PoincareJ::ExceptionType::Other);
}
}
