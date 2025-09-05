#include <escher/init.h>
#include <poincare/init.h>

#include "exam_mode_manager.h"
#include "global_preferences.h"
#include "init.h"
#include "shared/global_context.h"

namespace Apps {

void Init() {
  Shared::GlobalContext::Init();
  GlobalPreferences::Init();
  ExamModeManager::Init();
  Shared::GlobalContextAccessor::Init();
  Poincare::Init(GlobalPreferences::SharedGlobalPreferences(),
                 ExamModeManager::ExamModePtr(),
                 &Shared::GlobalContextAccessor::SequenceContext());
  Escher::Init(GlobalPreferences::SharedGlobalPreferences());
}

}  // namespace Apps
