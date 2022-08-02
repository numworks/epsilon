#include <shared/usb/calculator.h>

namespace Ion {
namespace Device {
namespace USB {

void Calculator::leave(uint32_t leaveAddress) {
  /* On N0120, the flasher runs with the power supply configured by ST
   * bootloader. This configuration cannot be changed without cycling power to
   * the device. As such, the flasher should not be used to jump to the
   * bootloader. Since this is the only use case, we do not allow the flasher
   * to leave. */
  assert(false);
}

}
}
}
