#include <bootloader/drivers/board.h>
#include <drivers/config/board.h>
#include <drivers/reset.h>
#include <shared/usb/calculator.h>

namespace Ion {
namespace Device {
namespace USB {

void Calculator::leave(uint32_t leaveAddress) {
  if (leaveAddress == 0 || leaveAddress == Board::Config::AXIMInterface) {
    Reset::core();
  }
  Board::executeIfAuthenticated(leaveAddress);
}

}
}
}
