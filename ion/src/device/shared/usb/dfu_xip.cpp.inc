#include <shared/drivers/usb.h>

#include "calculator.h"

namespace Ion {
namespace USB {

void DFU(DFUParameters parameters) {
  // Configure the kernel to avoid interrupting DFU protocole except on Back key
  Device::USB::willExecuteDFU();
  Device::USB::Calculator::PollAndReset(parameters);
  Device::USB::didExecuteDFU();
}

}  // namespace USB
}  // namespace Ion
