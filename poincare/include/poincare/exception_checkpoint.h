#ifndef POINCARE_EXCEPTION_CHECKPOINT_H
#define POINCARE_EXCEPTION_CHECKPOINT_H

#include <poincare/checkpoint.h>
#include <setjmp.h>

/* Usage: See comment in checkpoint.h
 *
 * To raise an error : ExceptionCheckpoint::Raise();
 *
 */

#define ExceptionRun(checkpoint) (CheckpointRun(checkpoint, setjmp(*(checkpoint.jumpBuffer())) != 0))

namespace Poincare {

class ExceptionCheckpoint final : public Checkpoint {
public:
  static void Raise() {
    assert(s_topmostExceptionCheckpoint != nullptr);
    s_topmostExceptionCheckpoint->rollback();
  }

  ExceptionCheckpoint();
  ~ExceptionCheckpoint();

  bool setActive(bool interruption);
  jmp_buf * jumpBuffer() { return &m_jumpBuffer; }
  static TreeNode * TopmostEndOfPoolBeforeCheckpoint() {
    return s_topmostExceptionCheckpoint ? s_topmostExceptionCheckpoint->getEndOfPoolBeforeCheckpoint() : nullptr;
  }
  static ExceptionCheckpoint * s_topmostExceptionCheckpoint;
private:
  void rollback() override;

  jmp_buf m_jumpBuffer;
  ExceptionCheckpoint * m_parent;
};

}

#endif
