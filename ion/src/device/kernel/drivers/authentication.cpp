#include <kernel/drivers/authentication.h>
#include <drivers/reset.h>
#include <kernel/warning_display.h>

namespace Ion {
namespace Device {
namespace Authentication {

static bool s_trustedUserland = true;

bool trustedUserland() {
  return s_trustedUserland;
}

void checkUserland(bool validKernelVersion) {
  s_trustedUserland = false; //BootloaderFunction::TrampolineisAuthentication(leaveAddress, KernelSize);
  if (s_trustedUserland) {
    if (validKernelVersion) {
      Reset::coreWhilePlugged();
    } else {
      //WarningDisplay::downgradeAttack();
      Reset::coreWhilePlugged();
    }
  } else {
    if (validKernelVersion) {
      // - shutdown the LED? Other decrease of privilege?
      WarningDisplay::unauthenticatedUserland();
      //Reset::jump(leaveAddress);
    } else {
      //WarningDisplay::obsoleteKernelRequired();
      Reset::coreWhilePlugged();
    }
  }
}

}
}
}
