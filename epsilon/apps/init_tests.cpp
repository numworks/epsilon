#include <escher/init.h>
#include <poincare/init.h>

#include "exam_mode_manager.h"
#include "global_preferences.h"
#include "init.h"
#include "shared/global_context.h"

namespace Apps {

void Init() {
  ::Shared::GlobalContext::s_sequenceStore.init();
  ::Shared::GlobalContext::s_sequenceCache.init(
      Shared::GlobalContext::s_sequenceStore.get());
  ::Shared::GlobalContext::s_continuousFunctionStore.init();

  GlobalPreferences::Init();
  ExamModeManager::Init();
  Shared::GlobalContextAccessor::Init();
  Poincare::Init(GlobalPreferences::SharedGlobalPreferences(),
                 ExamModeManager::ExamModePtr(),
                 &Shared::GlobalContextAccessor::Context());
  Escher::Init(GlobalPreferences::SharedGlobalPreferences());
}

}  // namespace Apps
