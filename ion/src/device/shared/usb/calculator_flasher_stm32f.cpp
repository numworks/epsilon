#include <flasher/drivers/usb.h>
#include <ion/reset.h>
#include <shared-core/drivers/board.h>
#include <shared-core/drivers/reset.h>

#include "calculator.cpp.inc"
#include "dfu_xip.cpp.inc"

namespace Ion {
namespace Device {
namespace USB {

/* This implementation is used by the flasher, which does not need to handle
 * privilege escalation */
void Calculator::leave(uint32_t leaveAddress) {
  Board::shutdown();
  Reset::jump(leaveAddress);
}

}  // namespace USB
}  // namespace Device
}  // namespace Ion
