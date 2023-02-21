#include <ion.h>
#include <shared/drivers/board_shared.h>
#include <shared/drivers/serial_number.h>
#include <shared/drivers/usb.h>
#include <shared/usb/calculator.h>

namespace Ion {
namespace USB {

void DFU() {
  // Configure the kernel to avoid interrupting DFU protocole except on Back key
  Device::USB::willExecuteDFU();
  Device::USB::Calculator::PollAndReset();
  Device::USB::didExecuteDFU();
}

}  // namespace USB
}  // namespace Ion
