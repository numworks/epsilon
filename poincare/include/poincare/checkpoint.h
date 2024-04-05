#ifndef POINCARE_CHECKPOINT_H
#define POINCARE_CHECKPOINT_H

/* Usage:
 *
 * CAUTION : A scope MUST be created directly around the Checkpoint, to ensure
 * to forget the Checkpoint once the interruptable code is executed. Indeed,
 * the scope calls the checkpoint destructor, which invalidate the current
 * checkpoint.
 * Also, any node stored under TopmostEndOfPool should not be altered.

void interruptableCode() {
  Poincare::Checkpoint cp;
  if (CheckpointRun(cp)) {
    CodeInvolvingLongComputationsOrLargeMemoryNeed();
  } else {
    ErrorHandler();
  }
}

*/

#define CheckpointRun(checkpoint, activation) (checkpoint.setActive(activation))

namespace Poincare {

class PoolObject;

class Checkpoint {
  friend class ExceptionCheckpoint;

 public:
  static PoolObject *TopmostEndOfPool() {
    return s_topmost ? s_topmost->m_endOfPool : nullptr;
  }

  Checkpoint();
  Checkpoint(const Checkpoint &) = delete;
  virtual ~Checkpoint() { protectedDiscard(); }
  Checkpoint &operator=(const Checkpoint &) = delete;

  PoolObject *const endOfPoolBeforeCheckpoint() { return m_endOfPool; }

  virtual void discard() const { protectedDiscard(); }

 protected:
  static Checkpoint *s_topmost;

  void rollback() const;
  void protectedDiscard() const;

  Checkpoint *const m_parent;

 private:
  virtual void rollbackException();

  PoolObject *const m_endOfPool;
};

}  // namespace Poincare

#endif
