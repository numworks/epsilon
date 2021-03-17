#include <kernel/drivers/authentication.h>
#include <drivers/reset.h>
#include <drivers/timing.h>
#include <kernel/warning_display.h>
#include <assert.h>

namespace Ion {
namespace Device {
namespace Authentication {

static bool s_trustedUserland = true;

bool trustedUserland() {
  return s_trustedUserland;
}

void updateTrustAndSwitchSlot(int currentKernelVersion, int newKernelVersion, int currentUserlandVersion, int newUserlandVersion) {
  s_trustedUserland = false; //BootloaderFunction::TrampolineisAuthentication(leaveAddress, KernelSize);
  if (currentKernelVersion > newKernelVersion || (s_trustedUserland && currentUserlandVersion > newUserlandVersion)) {
    WarningDisplay::obsoleteSoftware();
    Timing::msleep(5000);
    return;
  }
  if (!s_trustedUserland && currentKernelVersion < newKernelVersion) {
    WarningDisplay::kernelUpgradeRequired();
    Timing::msleep(5000);
    return;
  }
  if (s_trustedUserland) {
    assert(currentKernelVersion <= newKernelVersion && currentUserlandVersion <= newUserlandVersion);
    Reset::coreWhilePlugged();
  } else {
    assert(currentKernelVersion == newKernelVersion);
    // - shutdown the LED? Other decrease of privilege?
    WarningDisplay::unauthenticatedUserland();
    Timing::msleep(3000);
    //Reset::jump(leaveAddress);
  }
}

}
}
}
