#include "warning_display.h"
#include <drivers/keyboard.h>
#include <drivers/display.h>
#include <ion/display.h>
#include <kandinsky/rect.h>

namespace Ion {

/* For the kernel, we shortcut the SVC made by WarningDisplay to
 * direct calls to the right drivers. */

void Display::pullRect(KDRect r, KDColor * pixels) { Device::Display::pullRect(r, pixels); }

}

namespace Ion {
namespace Device {
namespace WarningDisplay {

void waitUntilKeyPress() {
  while (1) {
    Ion::Keyboard::State s = Keyboard::scan();
    if (s != 0) {
      break;
    }
  }
}

constexpr static int sUnauthenticatedUserlandNumberOfMessages = 7;
constexpr static const char * sUnauthenticatedUserlandMessages[sUnauthenticatedUserlandNumberOfMessages] = {
  "NON-AUTHENTICATED SOFTWARE",
  "Caution: you're using an",
  "unofficial software version.",
  "NumWorks can't be held responsible",
  "for any resulting damage.",
  "Some features (blue and green LED)",
  "are unavailable."
};

void unauthenticatedUserland() {
  showMessage(sUnauthenticatedUserlandMessages, sUnauthenticatedUserlandNumberOfMessages);
  waitUntilKeyPress();
}

constexpr static int sKernelUpgradeRequiredNumberOfMessages = 8;
constexpr static const char * sKernelUpgradeRequiredMessages[sKernelUpgradeRequiredNumberOfMessages] = {
  "OFFICIAL UPGRADE REQUIRED",
  "The software you were about",
  "to execute requires a more",
  "recent authentified kernel",
  "to be executed. Please upgrade",
  "your calculator with the last",
  "official software before trying",
  "again."
};

void upgradeRequired() {
  // TODO image instead of words
  showMessage(sKernelUpgradeRequiredMessages, sKernelUpgradeRequiredNumberOfMessages);
  waitUntilKeyPress();
}

constexpr static int sExternalAppsAvailableNumberOfMessages = 7;
constexpr static const char * sExternalAppsAvailableMessages[sExternalAppsAvailableNumberOfMessages] = {
  "EXTERNAL APPS",
  "You installed external applications",
  "to the official sofware.",
  "Some features may be therefore",
  "unavailable.",
  "Every permission will be restored",
  "when resetting the calculator.",
};

void externalAppsAvailable() {
  showMessage(sExternalAppsAvailableMessages, sExternalAppsAvailableNumberOfMessages);
  waitUntilKeyPress();
}

}
}
}
