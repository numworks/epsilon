#include "usb/calculator.h"
#include <ion.h>

namespace Ion {
namespace USB {

void DFU() {
  if (Ion::USB::Device::Calculator::PollAndReset()) {
    Ion::Device::jumpReset();
  }
}

}
}
