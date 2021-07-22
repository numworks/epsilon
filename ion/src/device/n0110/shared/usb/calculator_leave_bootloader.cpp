#include <bootloader/drivers/board.h>
#include <drivers/reset.h>
#include <shared/usb/calculator.h>

namespace Ion {
namespace Device {
namespace USB {

void Calculator::leave(uint32_t leaveAddress) {
  Board::executeIfAuthenticated(leaveAddress);
}

}
}
}
