#ifndef POINCARE_MEMORY_EXCEPTION_CHECKPOINT_H
#define POINCARE_MEMORY_EXCEPTION_CHECKPOINT_H

#include "checkpoint.h"
#include <setjmp.h>

/* Usage: See comment in checkpoint.h
 *
 * To raise an error : ExceptionCheckpoint::Raise();
 *
 */

#define ExceptionRun(checkpoint) (CheckpointRun(checkpoint, setjmp(*(checkpoint.jumpBuffer())) != 0))

namespace PoincareJ {

class ExceptionCheckpoint final : public Checkpoint {
public:
  static void Raise();

  ExceptionCheckpoint();
  ~ExceptionCheckpoint();

  bool setActive(bool interruption);
  jmp_buf * jumpBuffer() { return &m_jumpBuffer; }
private:
  void rollback() override;

  static ExceptionCheckpoint * s_topmostExceptionCheckpoint;

  jmp_buf m_jumpBuffer;
  ExceptionCheckpoint * m_parent;
};

}

#endif

