#include <config/board.h>
#include <drivers/board.h>
#include <shared-core/drivers/reset.h>
#include <shared/usb/calculator.h>

namespace Ion {
namespace Device {
namespace USB {

void Calculator::leave(uint32_t leaveAddress) {
  if (leaveAddress == 0 || leaveAddress == Board::Config::InternalFlashOrigin || leaveAddress == Board::Config::BootloaderOrigin) {
    Reset::core();
  }
  Board::executeIfAuthenticated(leaveAddress);
}

}
}
}
