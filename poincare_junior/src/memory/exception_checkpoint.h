#ifndef POINCARE_MEMORY_EXCEPTION_CHECKPOINT_H
#define POINCARE_MEMORY_EXCEPTION_CHECKPOINT_H

#include <setjmp.h>

#include "checkpoint.h"

// Usage:
#if 0
  ExceptionTry {
    /* Warning: Variable initialized before the checkpoint and modified here
     *          cannot be trusted after an exception has been raised. */
    // Default computations.
    ExceptionCheckpoint::Raise(ExceptionType::NonReal);
    if (true) {
      // Raising here will be handled in the following ExceptionCatch.
      ExceptionCheckpoint::Raise(ExceptionType::NonReal);
    }
  }
  ExceptionCatch(type) {
    // Raising here will be handled by parent ExceptionCatch.
    if (type != ExceptionType::NonReal) {
      // Unhandled exceptions should be raised to parent.
      ExceptionCheckpoint::Raise(type);
    }
    // Handle exceptions.
  }
#endif

#define ExceptionTry                \
  {                                 \
    ExceptionCheckpoint checkpoint; \
    checkpoint.setActive();         \
    if (setjmp(*(checkpoint.jumpBuffer())) == 0)

#define ExceptionCatch(typeVarName)                                   \
  }                                                                   \
  ExceptionType typeVarName = ExceptionCheckpoint::GetTypeAndClear(); \
  if (typeVarName != ExceptionType::None)

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

class ExceptionCheckpoint final : public Checkpoint {
 public:
  static void Raise(ExceptionType type);
  static ExceptionType GetTypeAndClear();

  ExceptionCheckpoint() : m_parent(s_topmostExceptionCheckpoint) {
    assert(s_exceptionType == ExceptionType::None);
  }
  ~ExceptionCheckpoint();

  void setActive() { s_topmostExceptionCheckpoint = this; }
  jmp_buf* jumpBuffer() { return &m_jumpBuffer; }

 private:
  void rollback();

  static ExceptionCheckpoint* s_topmostExceptionCheckpoint;
  static ExceptionType s_exceptionType;

  jmp_buf m_jumpBuffer;
  ExceptionCheckpoint* m_parent;
};

}  // namespace PoincareJ

#endif
