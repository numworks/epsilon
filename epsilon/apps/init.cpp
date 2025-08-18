#include "init.h"

#include <escher/init.h>
#include <poincare/init.h>

#include "apps_container_storage.h"
#include "exam_mode_manager.h"
#include "global_preferences.h"
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
                 Shared::GlobalContextAccessor::GlobalContext());
  Escher::Init(GlobalPreferences::SharedGlobalPreferences());

  ::AppsContainerStorage::sharedAppsContainerStorage.init();
}

}  // namespace Apps
