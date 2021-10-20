#include <drivers/reset.h>
#include <ion/reset.h>
#include <shared/usb/calculator.h>

namespace Ion {
namespace Device {
namespace USB {

/* This implementation is used by the flasher, which does not need to handle
 * priviledge escalation */
void Calculator::leave(uint32_t leaveAddress) {
  Reset::jump(leaveAddress);
}

}
}
}
