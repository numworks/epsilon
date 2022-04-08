#include <shared/usb/calculator.h>
#include <ion/reset.h>

namespace Ion {
namespace Device {
namespace USB {

void Calculator::leave(uint32_t) {
  /* Firmware built with this implementation should only use DFU to exchange
   * scripts and update Epsilon. As such, resetting to boot on the latest
   * version installed is the only form of leave needed. */
  Ion::Reset::core();
}

}
}
}
