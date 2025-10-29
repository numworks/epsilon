#include "runner_helpers.h"

#include <poincare/preferences.h>
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
  /* Check that preferences are at default values after each test.
   * translateBuiltins is the only field that can be edited on a
   * Poincare::Preferences::Interface */
  quiz_assert(Poincare::SharedPreferences->translateBuiltins() ==
              defaultTranslateBuiltin());
}
