#include <kernel/drivers/authentication.h>

namespace Ion {
namespace Device {
namespace Authentication {

static bool s_trustedUserland = true;

bool trustedUserland() {
  return s_trustedUserland;
}

bool updateTrust() {
  s_trustedUserland = false; //BootloaderFunction::TrampolineisAuthentication(leaveAddress, KernelSize);
  return s_trustedUserland;
}

}
}
}
