#include <ion/reset.h>

#include "svcall.h"

namespace Ion {
namespace Reset {

void SVC_ATTRIBUTES core(){SVC_RETURNING_VOID(SVC_RESET_CORE)}

ResetType SVC_ATTRIBUTES lastResetType() {
  SVC_RETURNING_R0(SVC_RESET_LAST_RESET_TYPE, ResetType)
}

}  // namespace Reset
}  // namespace Ion
