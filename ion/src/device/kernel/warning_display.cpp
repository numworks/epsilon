#include "warning_display.h"

namespace Ion {
namespace Device {
namespace WarningDisplay {

constexpr static int sNumberOfUnauthenticatedUserlandMessages = 7;
constexpr static const char * sUnauthenticatedUserlandMessages[sNumberOfUnauthenticatedUserlandMessages] = {
  "NON-AUTHENTICATED SOFTWARE",
  "Caution: you're using an",
  "unofficial software version.",
  "NumWorks can't be held responsible",
  "for any resulting damage.",
  "Some features (blue and green LED)",
  "are unavailable."
};

void unauthenticatedUserland() {
  showMessage(sUnauthenticatedUserlandMessages, sNumberOfUnauthenticatedUserlandMessages);
}

}
}
}
