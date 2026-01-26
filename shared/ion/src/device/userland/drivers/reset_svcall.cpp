#include <ion/exam_mode.h>
#include <ion/external_apps.h>
#include <ion/reset.h>

#include "svcall.h"

namespace Ion {
namespace Reset {

void SVC_ATTRIBUTES core_svcall() { SVC_RETURNING_VOID(SVC_RESET_CORE) }

void core() { core_svcall(); }

void coreDeleteApps() {
  Ion::ExternalApps::deleteApps(Ion::ExamMode::get().isActive(), false);
  core_svcall();
}

/* To be called when exam mode is already known to be active, to avoid calling
 * Ion::ExamMode::get() with an incoherent (exam mode)/(clearance level) pair.
 * This function is specifically used when setting exam mode from a firmware
 * with third-party apps. */
void coreActiveExamMode() {
  Ion::ExternalApps::deleteApps(true, false);
  core_svcall();
}

ResetType SVC_ATTRIBUTES lastResetType() {
  SVC_RETURNING_R0(SVC_RESET_LAST_RESET_TYPE, ResetType)
}

}  // namespace Reset
}  // namespace Ion
