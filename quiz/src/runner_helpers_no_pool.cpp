#include <ion/storage/file_system.h>
#include <poincare/old/context.h>
#include <poincare/src/memory/tree_stack.h>

#include "runner_helpers.h"

void flushGlobalData() {
  Poincare::Internal::SharedTreeStack->flush();
  quiz_assert(Poincare::Context::GlobalContext == nullptr);
  Ion::Storage::FileSystem::sharedFileSystem->destroyAllRecords();
}

void exception_run(void (*inner_main)(const char*, const char*, const char*),
                   const char* testFilter, const char* fromFilter,
                   const char* untilFilter) {
  inner_main(testFilter, fromFilter, untilFilter);
}
