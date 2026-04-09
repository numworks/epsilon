#pragma once

#include <assert.h>
#include <setjmp.h>

#include "block.h"
#include "tree_stack.h"

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

When TreeStackCheckpoint is raised without an active checkpoint, it raises an
ExceptionCheckpoint.
*/

#define ExceptionTryAfterBlock(rightmostBlock)      \
  {                                                 \
    TreeStackCheckpoint checkpoint(rightmostBlock); \
    checkpoint.setActive();                         \
    if (setjmp(*(checkpoint.jumpBuffer())) == 0)

#define ExceptionTry ExceptionTryAfterBlock(SharedTreeStack->lastBlock())

#define ExceptionCatchWithData(typeVarName, dataVarName)              \
  }                                                                   \
  uint8_t dataVarName = TreeStackCheckpoint::GetData();               \
  ExceptionType typeVarName = TreeStackCheckpoint::GetTypeAndClear(); \
  if (typeVarName != ExceptionType::None)

#define ExceptionCatch(typeVarName)                                     \
  }                                                                     \
  /* ExceptionCatchWithData should be use to explicitly discard data */ \
  assert(TreeStackCheckpoint::GetData() == 0);                          \
  ExceptionType typeVarName = TreeStackCheckpoint::GetTypeAndClear();   \
  if (typeVarName != ExceptionType::None)

namespace Poincare::Internal {

// All ExceptionType must be handled in ExceptionRunAndStoreExceptionType.
enum class ExceptionType : int {
  None = 0,
  // Memory exceptions
  TreeStackOverflow,
  IntegerOverflow,
  // Misc
  ParseFail,  // Used by parser, TODO: Use more distinct errors.
  SerializeBufferOverflow,
  SortFail,  // NAry::Sort encountered unsortable types with respect to Order
  NonPolynomial,  // Used by polynomial parser
  Other,          // Used internally for Unit tests.
};

class TreeStackCheckpoint final {
 public:
  static void Raise(ExceptionType type, uint8_t data = 0)
      __attribute__((__noreturn__));
  // To call before GetTypeAndClear
  static uint8_t GetData() { return s_exceptionData; }
  // This also clears exception data, GetData() should be called before.
  static ExceptionType GetTypeAndClear();

  TreeStackCheckpoint(Block* rightmostBlock);
  ~TreeStackCheckpoint();

  static bool HasActiveCheckpoint() {
    return s_topmostTreeStackCheckpoint != nullptr;
  }

  void setActive() { s_topmostTreeStackCheckpoint = this; }
  jmp_buf* jumpBuffer() { return &m_jumpBuffer; }

 private:
  void rollback();

  static TreeStackCheckpoint* s_topmostTreeStackCheckpoint;
  static ExceptionType s_exceptionType;
  // Used to store additional data about the exception when needed.
  static uint8_t s_exceptionData;

  jmp_buf m_jumpBuffer;
  TreeStackCheckpoint* m_parent;
  /* TODO: Assert no operation are performed on the TreeStack on blocks below
   * s_topmostTreeStackCheckpoint->m_rightmostBlock. */
  Block* m_rightmostBlock;
  /* We need to track the number of references to restore it on rollback
   * independently since TreeRefs created inside the Try pointing to old
   * elements would leak in the table. */
  uint16_t m_savedReferenceLength;
};

}  // namespace Poincare::Internal
