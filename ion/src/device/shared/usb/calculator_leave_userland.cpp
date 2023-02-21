#include <config/board.h>
#include <drivers/board.h>
#include <drivers/reset.h>
#include <shared/drivers/usb.h>
#include <shared/drivers/userland_header.h>
#include <shared/usb/calculator.h>

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
  Board::switchExecutableSlot(leaveAddress);
  USB::didExecuteDFU();
  /* The jump can't be done from switchExecutableSlot since we need to
   * terminate the interruption procedure before jumping. */
  Reset::jump(leaveAddress + sizeof(UserlandHeader));
}

}  // namespace USB
}  // namespace Device
}  // namespace Ion
