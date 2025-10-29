#include <apps/exam_mode_manager.h>
#include <apps/global_preferences.h>
#include <apps/init.h>
#include <apps/shared/global_store.h>
#include <ion/storage/file_system.h>
#include <poincare/exception_checkpoint.h>
#include <poincare/init.h>
#include <poincare/pool.h>

#include "quiz.h"
#include "runner_helpers.h"

class GlobalPreferencesTestBuilder {
 public:
  static void ResetGlobalPreferences() {
    *GlobalPreferences::s_data = GlobalPreferences::GlobalPreferencesData();
  }
};

void init() {
  // Initialize TreePool::sharedPool and TreeStack::SharedTreeStack
  Apps::Init();
}

void shutdown() { Poincare::Shutdown(); }

void flushGlobalData() {
  // Reset state that could have been altered
  GlobalPreferencesTestBuilder::ResetGlobalPreferences();
  if (ExamModeManager::ExamMode().isActive()) {
    ExamModeManager::SetExamMode(
        Poincare::ExamMode(Poincare::ExamMode::Ruleset::Off));
  }
  // Reset global stores
  Shared::GlobalContextAccessor::Store().resetAll();
  Shared::GlobalContextAccessor::SequenceStore().removeAll();
  Shared::GlobalContextAccessor::SequenceCache().resetCache();
  Shared::GlobalContextAccessor::ContinuousFunctionStore().removeAll();
  Ion::Storage::FileSystem::sharedFileSystem->destroyAllRecords();

  flushGlobalDataNoPool();
  // No PoolObject should leak after a unit test.
  quiz_assert(Poincare::Pool::sharedPool->numberOfObjects() == 0);
}

void exception_run(void (*inner_main)(const char*, const char*, const char*,
                                      int, int),
                   const char* testFilter, const char* fromFilter,
                   const char* untilFilter, int chunkId, int numberOfChunks) {
  Poincare::ExceptionCheckpoint ecp;
  if (ExceptionRun(ecp)) {
    inner_main(testFilter, fromFilter, untilFilter, chunkId, numberOfChunks);
  } else {
    // There has been a memory allocation problem
#if POINCARE_TREE_LOG
    Poincare::Pool::sharedPool->log();
#endif
    quiz_assert(false);
  }
}
