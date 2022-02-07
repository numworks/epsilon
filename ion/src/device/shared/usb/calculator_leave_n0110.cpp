#include <drivers/board.h>
#include <drivers/reset.h>
#include <ion/reset.h>
#include <shared/usb/calculator.h>

namespace Ion {
namespace Device {
namespace USB {

/* This implementation is used by the flasher, which does not need to handle
 * privilege escalation */
void Calculator::leave(uint32_t leaveAddress) {
  Board::shutdown();
  Reset::jump(leaveAddress);
}

}
}
}
