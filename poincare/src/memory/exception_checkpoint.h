#ifndef POINCARE_MEMORY_EXCEPTION_CHECKPOINT_H
#define POINCARE_MEMORY_EXCEPTION_CHECKPOINT_H

#include <assert.h>
#include <setjmp.h>

#include "block.h"

// Usage:
// ExceptionTry {
//   /* Warning: Variable initialized before the checkpoint and modified here
//    *          cannot be trusted after an exception has been raised. */
//   // Default computations.
//   if (something_goes_wrong) {
//     // Raising here will be handled in the following ExceptionCatch.
//     ExceptionCheckpoint::Raise(ExceptionType::PoolIsFull);
//   }
// }
// ExceptionCatch(type) {
//   // Raising here will be handled by parent ExceptionCatch.
//   if (type != ExceptionType::PoolIsFull) {
//     // Unhandled exceptions should be raised to parent.
//     ExceptionCheckpoint::Raise(type);
//   }
//   // Handle exceptions.
// }

#define ExceptionTryAfterBlock(rightmostBlock)      \
  {                                                 \
    ExceptionCheckpoint checkpoint(rightmostBlock); \
    checkpoint.setActive();                         \
    if (setjmp(*(checkpoint.jumpBuffer())) == 0)

#define ExceptionTry ExceptionTryAfterBlock(SharedTreeStack->lastBlock())

#define ExceptionCatch(typeVarName)                                   \
  }                                                                   \
  ExceptionType typeVarName = ExceptionCheckpoint::GetTypeAndClear(); \
  if (typeVarName != ExceptionType::None)

namespace Poincare::Internal {

// All ExceptionType must be handled in ExceptionRunAndStoreExceptionType.
enum class ExceptionType : int {
  None = 0,
  // Memory exceptions
  PoolIsFull,
  IntegerOverflow,
  RelaxContext,
  // Misc
  ParseFail,  // Used by parser, TODO: Use more distinct errors.
  Other,      // Used internally for Unit tests.
};

class ExceptionCheckpoint final {
 public:
  static void Raise(ExceptionType type) __attribute__((__noreturn__));
  static ExceptionType GetTypeAndClear();

  ExceptionCheckpoint(Block* rightmostBlock);
  ~ExceptionCheckpoint();

  void setActive() { s_topmostExceptionCheckpoint = this; }
  jmp_buf* jumpBuffer() { return &m_jumpBuffer; }

 private:
  void rollback();

  static ExceptionCheckpoint* s_topmostExceptionCheckpoint;
  static ExceptionType s_exceptionType;

  jmp_buf m_jumpBuffer;
  ExceptionCheckpoint* m_parent;
  /* TODO: Assert no operation are performed on the Edition pool on blocks below
   * s_topmostExceptionCheckpoint->m_rightmostBlock. */
  Block* m_rightmostBlock;
};

}  // namespace Poincare::Internal

#endif
