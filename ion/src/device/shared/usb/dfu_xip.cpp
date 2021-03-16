#include "calculator.h"
#include <drivers/board.h>
#include <drivers/serial_number.h>
#include <drivers/usb.h>
#include <shared/drivers/board.h>
#include <ion.h>

namespace Ion {
namespace Device {
namespace USB {

void DFU() {
  Board::shutdownInterruptions();
  Ion::Device::USB::Calculator::PollAndReset();
  Board::initInterruptions();
}

}
}
}
