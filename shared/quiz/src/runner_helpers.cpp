#include "runner_helpers.h"

#include <ion/storage/file_system.h>
#include <poincare/include/poincare/preferences.h>
#include <poincare/src/memory/tree_stack.h>

#include "quiz.h"

Poincare::Preferences::TranslateBuiltins defaultTranslateBuiltin() {
#if POINCARE_TRANSLATE_BUILTINS
  return Poincare::Preferences::TranslateBuiltins::TranslateToFrench;
#else
  return Poincare::Preferences::TranslateBuiltins::No;
#endif
}

void flushGlobalDataNoPool() {
  Poincare::Internal::SharedTreeStack->flush();
  quiz_assert(Poincare::Context::GlobalContext == nullptr);
  Ion::Storage::FileSystem::sharedFileSystem->destroyAllRecords();
  /* Check that preferences are at default values after each test.
   * translateBuiltins is the only field that can be edited on a
   * Poincare::Preferences::Interface */
  quiz_assert(Poincare::Preferences::SharedPreferences()->translateBuiltins() ==
              defaultTranslateBuiltin());
}
