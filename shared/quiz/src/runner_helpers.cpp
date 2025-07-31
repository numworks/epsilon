#include "runner_helpers.h"

#include <ion/storage/file_system.h>
#include <poincare/include/poincare/preferences.h>
#include <poincare/src/memory/tree_stack.h>

#include "quiz.h"

class PreferencesTestBuilder {
 public:
  static Poincare::Preferences::Instance* buildDefault() {
    static Poincare::Preferences::Instance defaultPreferences{};
    // Initialize the exam mode to "Off"
    defaultPreferences.examMode();
#if POINCARE_TRANSLATE_BUILTINS
    defaultPreferences.setTranslateBuiltins(
        Poincare::Preferences::TranslateBuiltins::TranslateToFrench);
#endif
    return &defaultPreferences;
  }
  static bool SharedPreferencesIsDefault() {
    return *static_cast<Poincare::Preferences::Instance*>(
               Poincare::Preferences::SharedPreferences()) ==
           *PreferencesTestBuilder::buildDefault();
  }
};

void flushGlobalDataNoPool() {
  Poincare::Internal::SharedTreeStack->flush();
  quiz_assert(Poincare::Context::GlobalContext == nullptr);
  Ion::Storage::FileSystem::sharedFileSystem->destroyAllRecords();
  // Check that preferences are at default values after each test
  quiz_assert(PreferencesTestBuilder::SharedPreferencesIsDefault());
}
