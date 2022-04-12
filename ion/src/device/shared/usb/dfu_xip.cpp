#include <drivers/board.h>
#include <drivers/serial_number.h>
#include <ion.h>
#include <shared/drivers/usb.h>
#include <shared/usb/calculator.h>

namespace Ion {
namespace USB {

Events::Event DFU() {
  // Configure the kernel to avoid interrupting DFU protocole except on Back key
  Device::USB::willExecuteDFU();
  Events::Event abortReason = Device::USB::Calculator::PollAndReset();
  Device::USB::didExecuteDFU();
  return abortReason;
}

}
}
