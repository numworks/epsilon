#include <escher/i18n.h>
#include <ion/storage/file_system.h>
#include <poincare/init.h>
#include <poincare/pool.h>
#include <quiz.h>

#include "exam_mode_manager.h"
#include "global_preferences.h"
#include "init.h"
#include "shared/global_store.h"

class GlobalPreferencesTestBuilder {
 public:
  static void ResetGlobalPreferences() {
    *GlobalPreferences::s_data = GlobalPreferences::GlobalPreferencesData();
  }
};

namespace Quiz {

void Env::Init() { Apps::Init(); }

void Env::Flush() {
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

  // No PoolObject should leak after a unit test.
  Poincare::Internal::SharedTreeStack->flush();
  quiz_assert(Poincare::Pool::sharedPool->numberOfObjects() == 0);
}

void Env::Shutdown() { Poincare::Shutdown(); }

}  // namespace Quiz

const char* I18n::translate(Message) { return nullptr; }
