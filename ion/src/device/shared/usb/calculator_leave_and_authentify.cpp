#include "calculator.h"
#include <ion/external_apps.h>
#include <shared/drivers/config/board.h>
#include <userland/drivers/board.h>
#include <userland/drivers/reset.h>

extern "C" {
extern char _external_apps_start;
}

namespace Ion {
namespace Device {
namespace USB {

void Calculator::leave(uint32_t leaveAddress) {
  if (leaveAddress >= reinterpret_cast<uint32_t>(&_external_apps_start) && leaveAddress < reinterpret_cast<uint32_t>(&_external_apps_start + Board::Config::ExternalAppsSectionLength)) {
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
