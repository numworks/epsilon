#include <kernel/drivers/authentication.h>
#include <drivers/reset.h>
#include <drivers/timing.h>
#include <kernel/drivers/board.h>
#include <kernel/warning_display.h>
#include <shared/drivers/config/external_flash.h>
#include <assert.h>

namespace Ion {
namespace Device {
namespace Authentication {

static bool s_trustedUserland = true;

bool trustedUserland() {
  return s_trustedUserland;
}

typedef void (*EntryPoint)();

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
    //Timing::msleep(3000);

    uint32_t userlandStartAddress = Board::userlandStart();
    // Pick the other userland slot
    if (userlandStartAddress < ExternalFlash::Config::StartAddress + ExternalFlash::Config::TotalSize/2) {
      userlandStartAddress += ExternalFlash::Config::TotalSize/2;
    } else {
      userlandStartAddress -= ExternalFlash::Config::TotalSize/2;
    }
    // TODO: tidy?
    Board::initInterruptions();
    Reset::jump(userlandStartAddress, false);
  }
}

}
}
}
