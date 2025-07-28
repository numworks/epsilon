#include <apps/global_preferences.h>
#include <apps/init.h>
#include <escher/init.h>
#include <poincare/exception_checkpoint.h>
#include <poincare/init.h>
#include <poincare/pool.h>

#include "quiz.h"
#include "runner_helpers.h"

class GlobalPreferencesTestBuilder {
 public:
  static GlobalPreferences buildDefault() { return GlobalPreferences(); }
};

void init() {
  // Initialize TreePool::sharedPool and TreeStack::SharedTreeStack
  Poincare::Init();
  Escher::Init();
  Apps::Init();

  // Initialize the exam mode to "Off" before running the tests
  Poincare::Preferences::SharedPreferences()->examMode();
}

void shutdown() { Poincare::Shutdown(); }

void flushGlobalData() {
  /* TODO: Some global data are asserted to be preserved while others are reset
   * here, unifomize expectations. */

  flushGlobalDataNoPool();
  quiz_assert(Poincare::Pool::sharedPool->numberOfObjects() == 0);
  quiz_assert(*GlobalPreferences::SharedGlobalPreferences() ==
              GlobalPreferencesTestBuilder::buildDefault());
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
