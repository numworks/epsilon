#include <ion/storage/file_system.h>
#include <poincare/old/exception_checkpoint.h>
#include <poincare/old/pool.h>
#include <poincare/src/memory/tree_stack.h>

#include "quiz.h"
#include "runner_helpers.h"

void flushGlobalData() {
  /* TODO: Only Pool and GlobalContext are expected to never leak. Uniformize
   * expectations. */
  quiz_assert(Poincare::Pool::sharedPool->numberOfNodes() == 0);
  quiz_assert(Poincare::Context::GlobalContext == nullptr);
  Poincare::Internal::SharedTreeStack->flush();
  Ion::Storage::FileSystem::sharedFileSystem->destroyAllRecords();
}

void exception_run(void (*inner_main)(const char*), const char* testFilter) {
  Poincare::ExceptionCheckpoint ecp;
  if (ExceptionRun(ecp)) {
    inner_main(testFilter);
  } else {
    // There has been a memory allocation problem
#if POINCARE_TREE_LOG
    Poincare::Pool::sharedPool->log();
#endif
    quiz_assert(false);
  }
}
