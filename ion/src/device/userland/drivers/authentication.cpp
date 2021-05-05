#include <ion/authentication.h>
#include <userland/drivers/svcall.h>

namespace Ion {
namespace Authentication {

bool SVC_ATTRIBUTES trustedUserland() {
  SVC_RETURNING_R0(SVC_AUTHENTICATION_TRUSTED_USERLAND, bool)
}

}
}
