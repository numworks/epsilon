#include "tree_stack_checkpoint.h"

#include <stdlib.h>

#include "tree_stack.h"

namespace Poincare::Internal {

TreeStackCheckpoint* TreeStackCheckpoint::s_topmostTreeStackCheckpoint;
ExceptionType TreeStackCheckpoint::s_exceptionType = ExceptionType::None;

TreeStackCheckpoint::TreeStackCheckpoint(Block* rightmostBlock)
    : m_parent(s_topmostTreeStackCheckpoint), m_rightmostBlock(rightmostBlock) {
  assert(s_exceptionType == ExceptionType::None);
}

TreeStackCheckpoint::~TreeStackCheckpoint() {
  assert((s_topmostTreeStackCheckpoint == this &&
          s_exceptionType == ExceptionType::None) ||
         (s_topmostTreeStackCheckpoint == m_parent &&
          s_exceptionType != ExceptionType::None));
  s_topmostTreeStackCheckpoint = m_parent;
}

void TreeStackCheckpoint::rollback() {
  // Next Raise will be handled by parent.
  s_topmostTreeStackCheckpoint = m_parent;
  /* Flush everything changed on the SharedTreeStack because it may be
   * corrupted. */
  SharedTreeStack->flushFromBlock(m_rightmostBlock);
  longjmp(m_jumpBuffer, 1);
}

void TreeStackCheckpoint::Raise(ExceptionType type) {
  assert(type != ExceptionType::None && s_exceptionType == ExceptionType::None);
  s_exceptionType = type;
  // Can't raise if there are no active TreeStackCheckpoints.
  if (s_topmostTreeStackCheckpoint == nullptr) {
    abort();
  }
  s_topmostTreeStackCheckpoint->rollback();
  abort();
}

ExceptionType TreeStackCheckpoint::GetTypeAndClear() {
  ExceptionType type = s_exceptionType;
  s_exceptionType = ExceptionType::None;
  return type;
}

}  // namespace Poincare::Internal

extern "C" {
void TreeStackCheckpointRaise() {
  Poincare::Internal::TreeStackCheckpoint::Raise(
      Poincare::Internal::ExceptionType::Other);
}
}
