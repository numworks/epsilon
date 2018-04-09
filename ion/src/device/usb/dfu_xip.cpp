#include "calculator.h"
#include "../device.h"

namespace Ion {
namespace USB {

void DFU() {
  if (Ion::USB::Device::Calculator::PollAndReset(true)) {
    Ion::Device::jumpReset();
  }
}

}
}
