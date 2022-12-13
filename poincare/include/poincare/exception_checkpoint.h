#ifndef POINCARE_EXCEPTION_CHECKPOINT_H
#define POINCARE_EXCEPTION_CHECKPOINT_H

#include <poincare/checkpoint.h>
#include <setjmp.h>

#define ExceptionRun(checkpoint) ( CheckpointRun(checkpoint, setjmp(*checkpoint.jumpBuffer())) != 0 )

namespace Poincare {

class ExceptionCheckpoint final : public Checkpoint {
public:
  static void Raise();
#if __EMSCRIPTEN__
  static bool HasBeenInterrupted();
  static void ClearInterruption();
#endif

  using Checkpoint::Checkpoint;

  jmp_buf * jumpBuffer() { return &m_jumpBuffer; }
  bool setActive(bool interruption);

private:
  void rollbackException() override;

  jmp_buf m_jumpBuffer;
};

}

#endif
