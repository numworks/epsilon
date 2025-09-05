#include "init.h"

#include <escher/init.h>
#include <poincare/init.h>

#include "apps_container_storage.h"
#include "exam_mode_manager.h"
#include "global_preferences.h"
#include "shared/global_store.h"

namespace Apps {

void Init() {
  Shared::GlobalStore::Init();
  GlobalPreferences::Init();
  ExamModeManager::Init();
  Shared::GlobalContextAccessor::Init();
  Poincare::Init(GlobalPreferences::SharedGlobalPreferences(),
                 ExamModeManager::ExamModePtr(),
                 &Shared::GlobalContextAccessor::SequenceContext());
  Escher::Init(GlobalPreferences::SharedGlobalPreferences());

  ::AppsContainerStorage::sharedAppsContainerStorage.init();
}

}  // namespace Apps
