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
  /* On N0120, the flasher runs with the power supply configured by ST
   * bootloader. This configuration cannot be changed without cycling power to
   * the device. As such, the flasher should not be used to jump to the
   * bootloader. */
}

}
}
}
