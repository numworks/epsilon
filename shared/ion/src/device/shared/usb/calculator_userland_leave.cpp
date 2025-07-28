#include <config/board.h>
#include <shared/drivers/userland_header.h>
#include <userland/drivers/board.h>
#include <userland/drivers/reset.h>
#include <userland/drivers/usb.h>

#include "calculator.h"

extern "C" {
extern char _external_apps_flash_start;
extern char _external_apps_flash_end;
}

namespace Ion {
namespace Device {
namespace USB {

void Calculator::leave(uint32_t leaveAddress) {
  if (leaveAddress >= reinterpret_cast<uint32_t>(&_external_apps_flash_start) &&
      leaveAddress < reinterpret_cast<uint32_t>(&_external_apps_flash_end)) {
    return;
  }
  Board::updateClearanceLevelForUnauthenticatedUserland(leaveAddress);
  USB::didExecuteDFU();
  /* The jump can't be done from the svcall since we need to terminate the
   * interruption procedure before jumping. */
  Reset::jump(leaveAddress + sizeof(UserlandHeader));
}

}  // namespace USB
}  // namespace Device
}  // namespace Ion
