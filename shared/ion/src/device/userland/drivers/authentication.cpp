#include <ion/authentication.h>

#include "svcall.h"

namespace Ion {
namespace Authentication {

ClearanceLevel SVC_ATTRIBUTES clearanceLevel() {
  SVC_RETURNING_R0(SVC_AUTHENTICATION_CLEARANCE_LEVEL, ClearanceLevel)
}

}  // namespace Authentication
}  // namespace Ion
