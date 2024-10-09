#ifndef POINCARE_POOL_CHECKPOINT_H
#define POINCARE_POOL_CHECKPOINT_H

/* Usage:
 *
 * CAUTION : A scope MUST be created directly around the PoolCheckpoint, to
 * ensure to forget the PoolCheckpoint once the interruptable code is executed.
 * Indeed, the scope calls the checkpoint destructor, which invalidate the
 * current checkpoint. Also, any node stored under TopmostEndOfPool should not
 * be altered.
 *
 *
 * void interruptableCode() {
 *   Poincare::PoolCheckpoint cp;
 *   if (CheckpointRun(cp)) {
 *     CodeInvolvingLongComputationsOrLargeMemoryNeed();
 *   } else {
 *     ErrorHandler();
 *   }
 * }
 *
 * PoolCheckpoint rollback will also flush the TreeStack so using a Tree* or a
 * TreeRef in a scope above a PoolCheckpoint is completely forbidden.
 */

#define CheckpointRun(checkpoint, activation) (checkpoint.setActive(activation))

namespace Poincare {

class PoolObject;

class PoolCheckpoint {
  friend class ExceptionCheckpoint;

 public:
  static PoolObject *TopmostEndOfPool() {
    return s_topmost ? s_topmost->m_endOfPool : nullptr;
  }

  PoolCheckpoint();
  PoolCheckpoint(const PoolCheckpoint &) = delete;
  virtual ~PoolCheckpoint() { protectedDiscard(); }
  PoolCheckpoint &operator=(const PoolCheckpoint &) = delete;

  PoolObject *const endOfPoolBeforeCheckpoint() { return m_endOfPool; }

  virtual void discard() const { protectedDiscard(); }

 protected:
  static PoolCheckpoint *s_topmost;

  void rollback() const;
  void protectedDiscard() const;

  PoolCheckpoint *const m_parent;

 private:
  virtual void rollbackException();

  PoolObject *const m_endOfPool;
};

}  // namespace Poincare

#endif
