#include "tree_stack_checkpoint.h"

#if POINCARE_EXCEPTION_CHECKPOINT
#include <poincare/include/poincare/exception_checkpoint.h>
#endif
#include <stdlib.h>

#include "tree_stack.h"

namespace Poincare::Internal {

TreeStackCheckpoint* TreeStackCheckpoint::s_topmostTreeStackCheckpoint;
ExceptionType TreeStackCheckpoint::s_exceptionType = ExceptionType::None;
uint8_t TreeStackCheckpoint::s_exceptionData = 0;

TreeStackCheckpoint::TreeStackCheckpoint(Block* rightmostBlock)
    : m_parent(s_topmostTreeStackCheckpoint),
      m_rightmostBlock(rightmostBlock),
      m_savedReferenceLength(SharedTreeStack->referenceTable()->length()) {
  assert(s_exceptionType == ExceptionType::None);
  assert(s_exceptionData == 0);
}

TreeStackCheckpoint::~TreeStackCheckpoint() {
  assert((s_topmostTreeStackCheckpoint == this &&
          s_exceptionType == ExceptionType::None && s_exceptionData == 0) ||
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
  SharedTreeStack->referenceTable()->setLength(m_savedReferenceLength);
  longjmp(m_jumpBuffer, 1);
}

void TreeStackCheckpoint::Raise(ExceptionType type, uint8_t data) {
  assert(type != ExceptionType::None &&
         s_exceptionType == ExceptionType::None && s_exceptionData == 0);
  s_exceptionType = type;
  s_exceptionData = data;
  if (!HasActiveCheckpoint()) {
#if POINCARE_EXCEPTION_CHECKPOINT
    // If there are no active TreeStackCheckpoints, raise an ExceptionCheckpoint
    s_exceptionType = ExceptionType::None;
    s_exceptionData = 0;
    ExceptionCheckpoint::Raise();
#endif
    abort();
  }
  // See ExceptionCheckpoint::Raise() for a comment on this assert
  assert((void*)&type < (void*)s_topmostTreeStackCheckpoint);
  s_topmostTreeStackCheckpoint->rollback();
  abort();
}

ExceptionType TreeStackCheckpoint::GetTypeAndClear() {
  ExceptionType type = s_exceptionType;
  s_exceptionType = ExceptionType::None;
  s_exceptionData = 0;
  return type;
}

}  // namespace Poincare::Internal
