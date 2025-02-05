#include "runner_helpers.h"

#include <ion/storage/file_system.h>
#include <poincare/include/poincare/preferences.h>
#include <poincare/src/memory/tree_stack.h>

#include "quiz.h"

void flushGlobalDataNoPool() {
  Poincare::Internal::SharedTreeStack->flush();
  quiz_assert(Poincare::Context::GlobalContext == nullptr);
  Ion::Storage::FileSystem::sharedFileSystem->destroyAllRecords();
  quiz_assert(*Poincare::Preferences::SharedPreferences() ==
              Poincare::Preferences());
}
