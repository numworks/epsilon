#include "calculator.h"
#include <drivers/board.h>
#include <drivers/serial_number.h>
#include <ion.h>

namespace Ion {
namespace USB {

void DFU() {
  Device::USB::Calculator::PollAndReset();
}

}
}
