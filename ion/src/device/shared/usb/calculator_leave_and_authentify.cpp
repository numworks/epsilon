#include "calculator.h"
#include <ion/external_apps.h>
#include <shared/drivers/config/board.h>
#include <userland/drivers/board.h>
#include <userland/drivers/reset.h>

namespace Ion {
namespace Device {
namespace USB {

void Calculator::leave(uint32_t leaveAddress) {
  uint32_t externalAppsSectionA = Board::Config::ExternalAppsAStartAddress ;
  uint32_t externalAppsSectionB = Board::Config::ExternalAppsBStartAddress ;
  if ((leaveAddress >= externalAppsSectionA && leaveAddress < externalAppsSectionA + Board::Config::ExternalAppsSectionLength) ||
     (leaveAddress >= externalAppsSectionB && leaveAddress < externalAppsSectionB + Board::Config::ExternalAppsSectionLength)) {
    Ion::ExternalApps::setVisible(true);
    if (Ion::ExternalApps::numberOfApps() > 0) {
      Board::downgradeTrustLevel(true); // Display pop-up
    } else {
      Ion::ExternalApps::setVisible(false);
    }
    return;
  }
  Board::switchExecutableSlot(leaveAddress);
  /* The jump can't be done from switchExecutableSlot since we need to
   * terminate the interruption procedure before jumping. */
  Reset::jump(leaveAddress);
}

}
}
}
