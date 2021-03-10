#include <kernel/drivers/authentication.h>

namespace Ion {
namespace Device {
namespace Authentication {

static bool s_trustedUserland = true;

bool trustedUserland() {
  return s_trustedUserland;
}

void untrustUserland() {
  s_trustedUserland = false;
}

}
}
}
