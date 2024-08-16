#ifndef POINCARE_MEMORY_TREE_STACK_CHECKPOINT_H
#define POINCARE_MEMORY_TREE_STACK_CHECKPOINT_H

#include <assert.h>
#include <setjmp.h>

#include "block.h"

/* Usage:

ExceptionTry {
  // Warning: Variable initialized before the checkpoint and modified here
  //          cannot be trusted after an exception has been raised.
  // Default computations.
  if (something_goes_wrong) {
    // Raising here will be handled in the following ExceptionCatch.
    TreeStackCheckpoint::Raise(ExceptionType::TreeStackOverflow);
  }
}
ExceptionCatch(type) {
  // Raising here will be handled by parent ExceptionCatch.
  if (type != ExceptionType::TreeStackOverflow) {
    // Unhandled exceptions should be raised to parent.
    TreeStackCheckpoint::Raise(type);
  }
  // Handle exceptions.
}

*/

#define ExceptionTryAfterBlock(rightmostBlock)      \
  {                                                 \
    TreeStackCheckpoint checkpoint(rightmostBlock); \
    checkpoint.setActive();                         \
    if (setjmp(*(checkpoint.jumpBuffer())) == 0)

#define ExceptionTry ExceptionTryAfterBlock(SharedTreeStack->lastBlock())

#define ExceptionCatch(typeVarName)                                   \
  }                                                                   \
  ExceptionType typeVarName = TreeStackCheckpoint::GetTypeAndClear(); \
  if (typeVarName != ExceptionType::None)

namespace Poincare::Internal {

// All ExceptionType must be handled in ExceptionRunAndStoreExceptionType.
enum class ExceptionType : int {
  None = 0,
  // Memory exceptions
  TreeStackOverflow,
  IntegerOverflow,
  RelaxContext,
  // Misc
  ParseFail,  // Used by parser, TODO: Use more distinct errors.
  SortFail,   // NAry::Sort encountered unsortable types with respect to Order
  Other,      // Used internally for Unit tests.
};

class TreeStackCheckpoint final {
 public:
  static void Raise(ExceptionType type) __attribute__((__noreturn__));
  static ExceptionType GetTypeAndClear();

  TreeStackCheckpoint(Block* rightmostBlock);
  ~TreeStackCheckpoint();

  void setActive() { s_topmostTreeStackCheckpoint = this; }
  jmp_buf* jumpBuffer() { return &m_jumpBuffer; }

 private:
  void rollback();

  static TreeStackCheckpoint* s_topmostTreeStackCheckpoint;
  static ExceptionType s_exceptionType;

  jmp_buf m_jumpBuffer;
  TreeStackCheckpoint* m_parent;
  /* TODO: Assert no operation are performed on the Edition pool on blocks below
   * s_topmostTreeStackCheckpoint->m_rightmostBlock. */
  Block* m_rightmostBlock;
  /* We need to track the number of references to restore it on rollback
   * independently since TreeRefs created inside the Try pointing to old
   * elements would leak in the table. */
  uint16_t m_savedReferenceLength;
};

}  // namespace Poincare::Internal

#endif
