#include "warning_display.h"

namespace Ion {
namespace Device {
namespace WarningDisplay {

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
}

constexpr static int sObsoleteSoftwareNumberOfMessages = 6;
constexpr static const char * sObsoleteSoftwareMessages[sObsoleteSoftwareNumberOfMessages] = {
  "OBSOLETE SOFTWARE",
  "The software you were about",
  "to execute is older than the",
  "current one.",
  "Only more recent software can",
  "be executed."
};

void obsoleteSoftware() {
  showMessage(sObsoleteSoftwareMessages, sObsoleteSoftwareNumberOfMessages);
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

void kernelUpgradeRequired() {
  showMessage(sKernelUpgradeRequiredMessages, sKernelUpgradeRequiredNumberOfMessages);
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
}

}
}
}
