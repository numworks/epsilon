#include <poincare/old/exception_checkpoint.h>
#include <poincare/old/pool.h>

#include "quiz.h"
#include "runner_helpers.h"

void flushGlobalData() {
  /* TODO: Only Pool and GlobalContext are expected to never leak. Uniformize
   * expectations. */
  flushGlobalDataNoPool();
  quiz_assert(Poincare::Pool::sharedPool->numberOfNodes() == 0);
}

void exception_run(void (*inner_main)(const char*, const char*, const char*),
                   const char* testFilter, const char* fromFilter,
                   const char* untilFilter) {
  Poincare::ExceptionCheckpoint ecp;
  if (ExceptionRun(ecp)) {
    inner_main(testFilter, fromFilter, untilFilter);
  } else {
    // There has been a memory allocation problem
#if POINCARE_TREE_LOG
    Poincare::Pool::sharedPool->log();
#endif
    quiz_assert(false);
  }
}
