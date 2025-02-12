#include "runner_helpers.h"

#include <apps/global_preferences.h>
#include <ion/storage/file_system.h>
#include <poincare/include/poincare/preferences.h>
#include <poincare/src/memory/tree_stack.h>

#include "quiz.h"

class PreferencesTestBuilder {
 public:
  static Poincare::Preferences buildDefault() {
    Poincare::Preferences defaultPreferences{};
    // Initialize the exam mode to "Off"
    defaultPreferences.examMode();
    return defaultPreferences;
  }
};

class GlobalPreferencesTestBuilder {
 public:
  static GlobalPreferences buildDefault() { return GlobalPreferences(); }
};

void flushGlobalDataNoPool() {
  Poincare::Internal::SharedTreeStack->flush();
  quiz_assert(Poincare::Context::GlobalContext == nullptr);
  Ion::Storage::FileSystem::sharedFileSystem->destroyAllRecords();
  // Check that preferences are at default values after each test
  quiz_assert(*Poincare::Preferences::SharedPreferences() ==
              PreferencesTestBuilder::buildDefault());
  quiz_assert(*GlobalPreferences::SharedGlobalPreferences() ==
              GlobalPreferencesTestBuilder::buildDefault());
}
