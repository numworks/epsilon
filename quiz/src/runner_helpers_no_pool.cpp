#include <ion/storage/file_system.h>
#include <poincare/src/memory/tree_stack.h>

#include "quiz.h"
#include "runner_helpers.h"

void flushGlobalData() {
  Poincare::Internal::SharedTreeStack->flush();
  Ion::Storage::FileSystem::sharedFileSystem->destroyAllRecords();
}

void exception_run(void (*inner_main)(const char*), const char* testFilter) {
  inner_main(testFilter);
}
