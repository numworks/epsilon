#include <bootloader/drivers/usb.h>
#include <config/board.h>
#include <drivers/board.h>
#include <shared-core/drivers/reset.h>

#include "calculator.cpp.inc"
#include "dfu_xip.cpp.inc"

namespace Ion {
namespace Device {
namespace USB {

void Calculator::leave(uint32_t leaveAddress) {
  if (leaveAddress == 0 || leaveAddress == Board::Config::InternalFlashOrigin ||
      leaveAddress == Board::Config::BootloaderOrigin) {
    Reset::core();
  }
  Board::executeIfAuthenticated(leaveAddress);
}

}  // namespace USB
}  // namespace Device
}  // namespace Ion
