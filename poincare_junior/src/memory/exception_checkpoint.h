#ifndef POINCARE_MEMORY_EXCEPTION_CHECKPOINT_H
#define POINCARE_MEMORY_EXCEPTION_CHECKPOINT_H

#include <setjmp.h>

#include "checkpoint.h"

// Usage:
#if 0
/* A scope MUST be created directly around the Checkpoint, to ensure
 * to forget the Checkpoint once the interruptable code is executed. Indeed,
 * the scope calls the checkpoint destructor, which invalidate the current
 * checkpoint. */
{
  /* Run the upcoming None switch statement. The first time an exception is
   * raised in this scope, jump back to this line and handle it in adequate
   * switch statement. The second time, parent exception will handle it. */
  ExceptionRunAndStoreExceptionTypeInVariableNamed(type);
  switch (type) {
    case ExceptionType::None:
      /* Warning: Variable initialized before the checkpoint and modified here
       *          cannot be trusted after an exception has been raised. */
      // Default computations.
      if (true) {
        // Raising in None statement will be handled in this switch.
        ExceptionCheckpoint::Raise(ExceptionType::NonReal);
      }
      break;
    // Raising in other statements will be handled by parent exception.
    case ExceptionType::PoolIsFull:
      // Handle exception.
      break;
    default:
      // Unhandled exceptions should be raised to parent.
      ExceptionCheckpoint::Raise(type);
  }
}
#endif

#define ExceptionRunAndStoreExceptionTypeInVariableNamed(typeVarName)   \
  ExceptionType typeVarName = ExceptionType::None;                      \
  ExceptionCheckpoint checkpoint;                                       \
  checkpoint.setActive();                                               \
  /* The invocation of setjmp must appear only in very strict contexts. \
   * One cannot do typeVarName = setjmp(*(checkpoint.jumpBuffer())) */  \
  switch (setjmp(*(checkpoint.jumpBuffer()))) {                         \
    case static_cast<int>(ExceptionType::None):                         \
      typeVarName = ExceptionType::None;                                \
      break;                                                            \
    case static_cast<int>(ExceptionType::PoolIsFull):                   \
      typeVarName = ExceptionType::PoolIsFull;                          \
      break;                                                            \
    case static_cast<int>(ExceptionType::IntegerOverflow):              \
      typeVarName = ExceptionType::IntegerOverflow;                     \
      break;                                                            \
    case static_cast<int>(ExceptionType::NonReal):                      \
      typeVarName = ExceptionType::NonReal;                             \
      break;                                                            \
    case static_cast<int>(ExceptionType::ZeroPowerZero):                \
      typeVarName = ExceptionType::ZeroPowerZero;                       \
      break;                                                            \
    case static_cast<int>(ExceptionType::ZeroDivision):                 \
      typeVarName = ExceptionType::ZeroDivision;                        \
      break;                                                            \
    case static_cast<int>(ExceptionType::Unhandled):                    \
      typeVarName = ExceptionType::Unhandled;                           \
      break;                                                            \
    case static_cast<int>(ExceptionType::Other):                        \
      typeVarName = ExceptionType::Other;                               \
      break;                                                            \
  }

namespace PoincareJ {

// All ExceptionType must be handled in ExceptionRunAndStoreExceptionType.
enum class ExceptionType : int {
  None = 0,
  // Memory exceptions
  PoolIsFull,
  IntegerOverflow,
  // Undefined result in given context
  NonReal,  // sqrt(-1), ln(-2), asin(2)
  // Undefined result
  ZeroPowerZero,  // 0^0 -> Should be ZeroDivision ?
  ZeroDivision,   // 1/0, tan(nπ/2)
  Unhandled,      // inf - inf, 0 * inf, unimplemented
  Other,          // Used internally for Unit tests.
};

static_assert(
    static_cast<int>(ExceptionType::None) == 0,
    "setjmp returns 0 when called by the original code (no exception)");

class ExceptionCheckpoint final : public Checkpoint {
 public:
  static void Raise(ExceptionType type);

  ExceptionCheckpoint() : m_parent(s_topmostExceptionCheckpoint){};
  ~ExceptionCheckpoint();

  void setActive() { s_topmostExceptionCheckpoint = this; }
  jmp_buf* jumpBuffer() { return &m_jumpBuffer; }

 private:
  void rollback(ExceptionType type);

  static ExceptionCheckpoint* s_topmostExceptionCheckpoint;

  jmp_buf m_jumpBuffer;
  ExceptionCheckpoint* m_parent;
};

}  // namespace PoincareJ

#endif
