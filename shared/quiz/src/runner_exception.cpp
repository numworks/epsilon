#include <poincare/exception_checkpoint.h>
#include <poincare/pool.h>

#include "runner.h"

namespace Quiz {

void Runner::exceptionRun() {
  Poincare::ExceptionCheckpoint checkpoint;
  if (ExceptionRun(checkpoint)) {
    innerRun();
  } else {
    // There has been a memory allocation problem
#if POINCARE_TREE_LOG
    Poincare::Pool::sharedPool->log();
#endif
    quiz_assert(false);
  }
}

}  // namespace Quiz
