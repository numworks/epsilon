#ifndef POINCARE_EXCEPTION_CHECKPOINT_H
#define POINCARE_EXCEPTION_CHECKPOINT_H

#include <setjmp.h>

#include <new>

#include "pool_checkpoint.h"

/* WARNING:
 * ExceptionCheckpoint is raised when a TreeStackCheckpoint is raised without an
 * active checkpoint. Therefore, an ExceptionCheckpoint cannot be set below an
 * active TreeStackCheckpoint. */

#define ExceptionRun(checkpoint) \
  (CheckpointRun(checkpoint, setjmp(*checkpoint.jumpBuffer())) != 0)

namespace Poincare {

class ExceptionCheckpoint final : public PoolCheckpoint {
 public:
  static void Raise();

  using PoolCheckpoint::PoolCheckpoint;

  jmp_buf* jumpBuffer() { return &m_jumpBuffer; }
  bool setActive(bool interruption);

 private:
  void rollbackException() override;

  jmp_buf m_jumpBuffer;
};

}  // namespace Poincare

#endif
