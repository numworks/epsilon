#include <ion/reset.h>
#include <shared/usb/calculator.h>

namespace Ion {
namespace Device {
namespace USB {

void Calculator::leave(uint32_t) {
  /* Firmware built with this implementation should only use DFU to exchange
   * scripts and update Epsilon. As such, resetting to boot on the latest
   * version installed is the only form of leave needed. */
  Ion::Reset::core();
}

}  // namespace USB
}  // namespace Device
}  // namespace Ion
