#include <ion/exam_mode.h>
#include <ion/external_apps.h>
#include <ion/reset.h>

#include "svcall.h"

namespace Ion {
namespace Reset {

void SVC_ATTRIBUTES core_svcall() { SVC_RETURNING_VOID(SVC_RESET_CORE) }

void core() {
  Ion::ExternalApps::deleteApps(Ion::ExamMode::get().isActive(), false);
  core_svcall();
}

ResetType SVC_ATTRIBUTES lastResetType() {
  SVC_RETURNING_R0(SVC_RESET_LAST_RESET_TYPE, ResetType)
}

}  // namespace Reset
}  // namespace Ion
